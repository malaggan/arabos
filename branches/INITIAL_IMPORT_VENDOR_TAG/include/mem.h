#ifndef MEM_H
#define MEM_H

#ifdef _cplusplus
extern "C" {
#endif

#include <sys.h>

// should this be defined in <gdt.h> ??
#define NULL (0)

// copies string of bytes of lenght `count'
void *memcpy(unsigned char *dest, const unsigned char *src, size_t count);

// fills the memory area with `val'
void *memset(unsigned char *dest, char val, size_t count);

// fills the memory area with `val'; used for words
inline unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);

#ifdef _cplusplus
}
#endif

#endif /*MEM_H*/
