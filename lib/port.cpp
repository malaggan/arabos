#include <port.h>
#include <asm.h>
unsigned char inportb (unsigned short _port)
{
	unsigned char retval;
	ASM (IO_WAIT "inb %1, %0" : "=a" (retval) : "d" (_port));
	return retval;
}

void outportb (unsigned short _port, char _data)
{
	ASM ("outb %1, %0\n" IO_WAIT : : "d" (_port), "a" (_data));
}

unsigned short inportw (unsigned short _port)
{
	unsigned char retval;
	ASM (IO_WAIT "insw" :: "D" (&retval), "d" (_port) : "memory");
	return retval;
}

void outportw (unsigned short _port, short _data)
{
	ASM ("outw %1, %0\n" IO_WAIT :: "d" (_port), "a" (_data));
}
