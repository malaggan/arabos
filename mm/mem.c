#include <mem.h>

// copies string of bytes of lenght `count'
inline void *memcpy(unsigned char *dest, const unsigned char *src, size_t count)
{
	const char *sp = (const char *)src;
	char *dp = (char *)dest;
	for(; count != 0; count--) *dp++ = *sp++;
	return dest;
}

// fills the memory area with `val'
inline void *memset(unsigned char *dest, char val, size_t count)
{
	char *temp = (char *)dest;
	for( ; count != 0; count--) *temp++ = val;
	return dest;
}

// fills the memory area with `val'; used for words
inline unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
	unsigned short *temp = (unsigned short *)dest;
	for( ; count != 0; count--) *temp++ = val;
	return dest;
}
