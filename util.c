#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

void die(char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "bfc: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(EXIT_FAILURE);
}
