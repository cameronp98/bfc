#ifndef UTIL_H
#define UTIL_H

void die(const char *fmt, ...);

char *changeExtension(const char *path, const char *ext);

int fsystem(const char *fmt, ...);

#endif
