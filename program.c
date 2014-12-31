#include "error.h"
#include "program.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern size_t bfc_stack_size;

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

	Operation *op = Operation_new(type, data);

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

Operation *Operation_new(char type, int data)
{
	Operation *op = malloc(sizeof *op);
	if (op == NULL)
		die("Operation_new(): memory error");
	op->type = type;
	op->data = data;
	return op;
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

		int opcount = 1;
		while ((nextc = getc(fp)) == c)
			opcount++;

		switch(c)
		{
			case '+':
			case '-':
			case '<':
			case '>':
			case '.':
				Program_put(p, c, opcount);
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

void Program_reduce(Program *p, char *pattern, ReduceCallback *cb)
{

	const size_t pat_len = strlen(pattern);
	// last valid search index
	size_t max_idx = p->size - (pat_len - 1);


	/* TODO: find a better algorithm! */
	// identify all instances of substring `pattern` and replace them in the
	//  program with the replacement specified by the callback
	int i, j;
	for (i = 0; i < max_idx; i++)
	{
		int match = 1;
		for (j = 0; j < pat_len; j++)
		{
			if (p->data[i+j]->type != pattern[j]) {
				match = 0;
				break;
			}
		}

		if (match)
		{
			int k;

			// replace the sequence in the program with the result from cb
			p->data[i] = (*cb)(p->data+i);

			for (k = 1; k < p->size - i; k++) {
				p->data[i+k] = p->data[i+k+pat_len];
			}

			// recalculate the last valid search index +  new program size
			p->size = max_idx - 1;
			max_idx -= pat_len - 1;

			// resize
			p->data = realloc(p->data, p->size * sizeof(*p->data));
			if (p->data == NULL)
				die("Program_reduce(): memory error");

		}
	}

}
