#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#define MAX_CMD_SIZE 128

/* print formatted message and exit */
void die(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	fprintf(stderr, "bfc: ");
	vfprintf(stderr, fmt, args);
	fprintf(stderr, "\n");
	va_end(args);
	exit(EXIT_FAILURE);
}

/* system() with string formatting */
int fsystem(const char *fmt, ...) {
	va_list argptr;
	va_start(argptr, fmt);
	char command[MAX_CMD_SIZE] = {0};
	vsnprintf(command, MAX_CMD_SIZE, fmt, argptr);
	return system(command);
}

char *changeExtension(const char *path, const char *ext)
{
	/* find the last dot (if any) and replace it with 0, append `ext`*/
	const size_t ext_len = strlen(ext);
	const char *dot = strchr(path, '.');
	if (dot == NULL)
		dot = path + strlen(path);
	unsigned long dot_idx = dot - path;
	char *ret = malloc(dot_idx + ext_len + 1);
	strncpy(ret, path, dot_idx);
	ret[dot_idx] = 0;
	strncat(ret, ext, ext_len);
	return ret;
}
