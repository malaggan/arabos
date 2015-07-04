#pragma once
#include <types.h>

int strnlen(const char *str, int max);
int strncmp(const char *s1, const char* s2, int max);

/* Return a new identical string.  */
char * strdup (const char *str);
char * strndup (const char *str, size_t at_most);

void itoa (char *buf, int base, int d); // int to ascii
