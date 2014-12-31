#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>
#include <stdio.h>

void die(char *fmt, ...);

void *bfc_alloc(void *ptr, size_t size);

FILE *bfc_fopen(const char *path, const char *mode);

#endif
