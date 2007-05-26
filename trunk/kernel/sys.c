/* Copyright (C) 2007  Mohammad Nabil 
mohammad (dot) nabil (dot) h (at) gmail (dot) com

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.*/

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
