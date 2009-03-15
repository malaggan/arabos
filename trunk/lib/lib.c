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

#include <asm.h>
#include <lib.h>

inline unsigned char *memcpy(unsigned char *dest, const unsigned char *src, size_t count)
{
	ASM("cld\n"
            "rep\n"
            "movsb" :: "S"(src), "D"(dest), "c"(count));
	return dest;
}

inline unsigned short *memcpyw(unsigned short *dest, const unsigned short *src, size_t count)
{
	ASM("cld\n"
            "rep\n"
            "movsw" :: "S"(src), "D"(dest), "c"(count));
	return dest;
}

inline unsigned char *memset(unsigned char *dest, char val, size_t count)
{
	ASM("cld\n"
            "rep\n"
            "stosb" :: "a"(val), "D"(dest), "c"(count));
	return dest;
}

inline unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count)
{
	ASM("cld\n"
            "rep\n"
            "stosw" :: "a"(val), "D"(dest), "c"(count));
	return dest;
}

unsigned char inportb (unsigned short _port)
{
	unsigned char retval;
	IO_WAIT();
	ASM ("inb %1, %0" : "=a" (retval) : "dN" (_port));
	return retval;
}

void outportb (unsigned short _port, char _data)
{
	ASM ("outb %1, %0" : : "dN" (_port), "a" (_data));
	IO_WAIT();
}

/* Convert the integer D to a string and save the string in BUF. If
BASE is equal to 'd', interpret that D is decimal, and if BASE is
equal to 'x', interpret that D is hexadecimal. */
void
itoa (char *buf, int base, int d)
{
	char *p = buf;
	char *p1, *p2;
	unsigned long ud = d;
	int divisor = 10;

	/* If %d is specified and D is minus, put `-' in the head. */
	if (base == 'd' && d < 0)
	{
		*p++ = '-';
		buf++;
		ud = -d;
	}
	else if (base == 'x')
		divisor = 16;

	/* Divide UD by DIVISOR until UD == 0. */
	do
	{
		int remainder = ud % divisor;

		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
	}
	while (ud /= divisor);

	/* Terminate BUF. */
	*p = 0;

	/* Reverse BUF. */
	p1 = buf;
	p2 = p - 1;
	while (p1 < p2)
	{
		char tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

inline int strnlen(const char *str, int max)
{
	int c = 0;

	while(*str++ && c < max) c++;

	return c;
}

/*
inline int strcmp(const char *s1,const char* s2)
{
	if(s1 == s2)
		return(0);
	while(*s1 == *s2++)
		if(*s1++ == '\0')
			return(0);
	return(*s1 - s2[-1]);
}
*/

inline int strncmp(const char *s1,const char* s2, int len)
{
	if(s1 == s2)
		return(0);
	while(*s1 == *s2++ && len--)
		if(*s1++ == '\0')
			return(0);
	return(*s1 - s2[-1]);
}

/*
void abort(int exit_code)
{

}

void __assert(int s)
{

}
*/

