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

#ifndef MEM_H
#define MEM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <sys.h>

// should this be defined in <gdt.h> ??
#define NULL (0)

// copies string of bytes of lenght `count'
void *memcpy(unsigned char *dest, const unsigned char *src, size_t count);

// copies string of words of lenght `count'
void *memcpyw(unsigned short *dest, const unsigned short *src, size_t count);

// fills the memory area with `val'
void *memset(unsigned char *dest, char val, size_t count);

// fills the memory area with `val'; used for words
inline unsigned short *memsetw(unsigned short *dest, unsigned short val, size_t count);

#ifdef __cplusplus
}
#endif

#endif /*MEM_H*/
