#include <sys.h>

/* We will use this later on for reading from the I/O ports to get data
*	from devices such as the keyboard. We are using what is called
*	'inline assembly' in these routines to actually do the work */
unsigned char inportb (unsigned short _port)
{
	unsigned char retval;
	IO_WAIT();
	ASM ("inb %1, %0" : "=a" (retval) : "dN" (_port));
	return retval;
}

/* We will use this to write to I/O ports to send bytes to devices. This
*	will be used in the next tutorial for changing the textmode cursor
*	position. Again, we use some inline assembly for the stuff that simply
*	cannot be done in C */
void outportb (unsigned short _port, char _data)
{
	ASM ("outb %1, %0" : : "dN" (_port), "a" (_data));
	IO_WAIT();
}

void abort(int exit_code)
{

}

void __assert(int s)
{

}
