/*
 * strings.h
 *
 * Definitions for string operations.
 */

#ifndef _STRINGS_H_
#define _STRINGS_H_

#include <sys/types.h> /* for size_t */

int ffs(int);
int strcasecmp(const char *, const char *);
int strncasecmp(const char *, const char *, size_t);

#endif /* _STRINGS_H_ */
