#include <memops.h>
#include <types.h>
#include <asm.h>

void *memcpy(void *dest, const void *src, size_t count)
{
	ASM("cld\n"
	    "rep\n"
	    "movsb"
	    ::
	     "S"(reinterpret_cast<const uint8_t*>(src)),
	     "D"(reinterpret_cast<uint8_t*>(dest)),
	     "c"(count));
	return dest;
}

void *memcpyw(void *dest, const void *src, size_t count)
{
	ASM("cld\n"
	    "rep\n"
	    "movsw"
	    ::
	     "S"(reinterpret_cast<const uint16_t*>(src)),
	     "D"(reinterpret_cast<uint16_t*>(dest)),
	     "c"(count));
	return dest;
}

unsigned char *memset(unsigned char *dest, char val, size_t count)
{
	ASM("cld\n"
	    "rep\n"
	    "stosb" :: "a"(val), "D"(dest), "c"(count));
	return dest;
}

unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
	ASM("cld\n"
	    "rep\n"
	    "stosw" :: "a"(val), "D"(dest), "c"(count));
	return dest;
}
