#ifndef STRING_H
#define STRING_H

#ifdef _cplusplus
extern "C" {
#endif

#include <sys.h>

// integer to ascii
void itoa (char *buf, int base, int d);

inline int strlen(const char *str);

#ifdef _cplusplus
}
#endif

#endif /*STRING_H*/
