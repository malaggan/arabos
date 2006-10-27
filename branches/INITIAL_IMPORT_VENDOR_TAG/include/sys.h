#ifndef SYS_H
#define SYS_H

#ifdef _cplusplus
extern "C" {
#endif

// we _really_ need to avoid infinite loops in kernel mode!
#define MAX_CHARS 1024
extern int printed_chars; // counter to compare for MAX_CHARS 

#define size_t int

#include <mem.h> // for NULL

unsigned char inportb (unsigned short _port);
void outportb (unsigned short _port, char _data);

#define ASM __asm__ __volatile__

#define IO_WAIT() \
		ASM("nop"); ASM("nop"); ASM("nop"); ASM("nop");

#ifdef _cplusplus
}
#endif

#endif /*SYS_H*/
