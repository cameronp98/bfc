#include "util.h"
#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern size_t bfc_stack_size;
extern size_t bfc_buffer_size;

static Program *Program_new(void)
{
	Program *p = malloc(sizeof *p);
	if (p == NULL)
		die("Program_new(): memory error");
	p->data = NULL;
	p->size = 0;
	return p;
}

static void Program_put(Program *p, char type, int data)
{

	Operation op = {type, data};

	if (p->size == 0)
	{
		p->data = malloc(sizeof(*p->data));
		p->data[0] = op;
	} else {
		p->data = realloc(p->data, (p->size + 1) * sizeof(*p->data));
		p->data[p->size] = op;
	}

	p->size++;

}

Program *Program_fromFile(FILE *fp)
{

	size_t *stack = malloc(bfc_stack_size * sizeof(*stack));
	if (stack == NULL)
		die("Program_fromFile(): memory error");
	size_t *sp = stack;

	size_t loop_counter = 0;

	Program *p = Program_new();

	char c, nextc;
	for (c = getc(fp); c != EOF; c = nextc)
	{

		unsigned opcount = 1;
		while ((nextc = getc(fp)) == c)
			opcount++;

		switch(c)
		{
			case '+':
			case '-':
			case '<':
			case '>':
				Program_put(p, c, opcount);
			case '.':
				while (opcount--) {
					Program_put(p, c, 0);
				}
				break;
			case ',':
				Program_put(p, c, 0);
				break;
			case '[':
				while (opcount--) {
					if (sp == (stack + bfc_stack_size))
						die("stack overflow");
					*++sp = ++loop_counter;
					Program_put(p, c, loop_counter);
				}
				break;
			case ']':
				while (opcount--) {
					if (sp == stack)
						die("stack underflow");
					Program_put(p, c, *sp--);
				}
				break;
		}

	}

	int stack_empty = (sp == stack);

	free(stack);

	if (!stack_empty) {
		free(p);
		die("unmatched '['");
	}

	return p;
}

void Program_free(Program *p)
{
	if (p->size > 0)
		free(p->data);
	free(p);
}

void Program_reduce(Program *p, const char *pattern, ReduceCallback *cb)
{

	const size_t pat_len = strlen(pattern);

	/* TODO: find a better algorithm! */
	// identify all instances of substring `pattern` and replace them in the
	//  program with the replacement specified by the callback
	for (int i = 0; i < p->size; i++)
	{
		int match = 1;
		for (int j = 0; j < pat_len; j++)
		{
			if (p->data[i+j].type != pattern[j]) {
				match = 0;
				break;
			}
		}

		if (match)
		{
			// replace the sequence in the program with the result from cb
			p->data[i] = (*cb)(p->data+i);

			// shift the rest of the program left if necessary
			if (pat_len > 1)
			{
				/* TODO: find out why memcpy breaks the program */
				for (int k = 0; k < p->size - (i + 1); k++) {
					p->data[i+1+k] = p->data[i+pat_len+k];
				}

				p->size -= (pat_len - 1);

				p->data = realloc(p->data, p->size * sizeof(*p->data));
				if (p->data == NULL)
					die("Program_reduce(): memory error");
			}
		}
	}

}

void Program_compile(Program *p, const char *base_path)
{

	char *asm_path = changeExtension(base_path, ".s");

	FILE *fp = fopen(asm_path, "w");
	if (fp == NULL)
		die("failed to open output file");

	/* assembly headers: initialise sections, buffer, registers and io funcs */
	// buffer (tape)
	fprintf(fp, "section .bss\n");
	fprintf(fp, "\tbuffer resb %zu\n\n", bfc_buffer_size);
	// text section
	fprintf(fp, "section .text\n\n");
	// getb: read 1 byte from stdin
	fprintf(fp, "getb:\n");
	fprintf(fp, "\tpush ebx\n");
	fprintf(fp, "\tmov ecx,ebx\n");
	fprintf(fp, "\tmov eax,3\n");
	fprintf(fp, "\tmov ebx,0\n");
	fprintf(fp, "\tmov edx,1\n");
	fprintf(fp, "\tint 80h\n");
	fprintf(fp, "\tpop ebx\n");
	fprintf(fp, "\tret\n\n");
	// putb: write 1 byte to stdout
	fprintf(fp, "putb:\n");
	fprintf(fp, "\tpush ebx\n");
	fprintf(fp, "\tmov ecx,ebx\n");
	fprintf(fp, "\tmov eax,4\n");
	fprintf(fp, "\tmov ebx,1\n");
	fprintf(fp, "\tmov edx,1\n");
	fprintf(fp, "\tint 80h\n");
	fprintf(fp, "\tpop ebx\n");
	fprintf(fp, "\tret\n\n");
	// main()
	fprintf(fp, "global _start\n");
	fprintf(fp, "_start:\n");
	fprintf(fp, "\tmov ebx,buffer\n\n");

	Operation prev_op = {-1, -1};

	for (int i = 0; i < p->size; i++)
	{
		Operation op = p->data[i];

		char next_type = i < (p->size - 1) ? p->data[i + 1].type : -1;

		switch(op.type)
		{
			case '+':
				// save the add value but don't write the command
				if (next_type != '%') {
					if (prev_op.type == ']' || prev_op.type == -1)
						fprintf(fp, "\tmov byte [ebx],%d\n", op.data);
					else if (op.data == 1)
						fprintf(fp, "\tinc byte [ebx]\n");
					else
						fprintf(fp, "\tadd byte [ebx],%d\n", op.data);
				}
				break;
			case '-':
				if (op.data == 1)
					fprintf(fp, "\tdec byte [ebx]\n");
				else
					fprintf(fp, "\tsub byte [ebx],%d\n", op.data);
				break;
			case '>':
				if (op.data == 1)
					fprintf(fp, "\tdec ebx\n");
				else
					fprintf(fp, "\tsub ebx,%d\n", op.data);
				break;
			case '<':
				if (op.data == 1)
					fprintf(fp, "\tinc ebx\n");
				else
					fprintf(fp, "\tadd ebx,%d\n", op.data);
				break;
			case '[':
				if (prev_op.type != '+') {
					fprintf(fp, "\tcmp byte [ebx],0\n");
					fprintf(fp, "\tjz .E%d\n\n", op.data);
				}
				fprintf(fp, "\t.S%d:\n", op.data);
				break;
			case ']':
				if (prev_op.type == '+') {
					fprintf(fp, "\tjmp .S%d\n\n", op.data);
				} else {
					fprintf(fp, "\tcmp byte [ebx],0\n");
					fprintf(fp, "\tjnz .S%d\n\n", op.data);
				}
				fprintf(fp, "\t.E%d:\n", op.data);
				break;
			case ',':
				fprintf(fp, "\tcall getb\n");
				break;
			case '.':
				fprintf(fp, "\tcall putb\n");
				break;
			case '%':
				if (op.data == 1) {
					fprintf(fp, "mov byte [ebx],%d\n", op.data);
				} else {
					if (i > 0 && prev_op.type == '+')
						fprintf(fp, "\tmov al,%d\n", prev_op.data);
					else
						fprintf(fp, "\tmov ax,byte [ebx]\n");
					fprintf(fp, "\tmov dl,%d\n", op.data);
					fprintf(fp, "\tdiv dl\n");
					fprintf(fp, "\tmov byte [ebx],ah\n");
				}
				break;
		}

		prev_op = op;

	}

	// return 0
	fprintf(fp, "\n\tmov eax,1\n");
	fprintf(fp, "\txor ebx, ebx\n");
	fprintf(fp, "\tint 80h\n");

	fclose(fp);

	if (fsystem("nasm -f elf %s", asm_path) == 0) {
		fsystem("ld -m elf_i386 %s.o -o %s", base_path, base_path);
		fsystem("rm -f %s.o", base_path);
	}

	free(asm_path);

}
