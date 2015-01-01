#ifndef PROGRAM_H
#define PROGRAM_H

#include <stdio.h>
#include <stddef.h>

typedef struct
{
	char type;
	int data;
} Operation;

typedef struct
{
	Operation *data;
	size_t size;
} Program;

typedef Operation (ReduceCallback)(Operation *ops);

Operation *Operation_new(char type, int data);

Program *Program_fromFile(FILE *fp);

void Program_free(Program *p);

/*
 * Find the sequence of operations in `p` that matches `pattern` (e.g. "[-]")
 * Pass the sequence to `cb` and replace the sequence in the program with the
 *  single operator returned by `cb`
 */
void Program_reduce(Program *p, char *pattern, ReduceCallback *cb);

#endif
