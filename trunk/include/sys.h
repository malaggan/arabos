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

#ifndef SYS_H
#define SYS_H

#ifdef __cplusplus
extern "C" {
#endif

// we _really_ need to avoid infinite loops in kernel mode!
#define MAX_CHARS 1024
extern int printed_chars; // counter to compare for MAX_CHARS 

#define size_t int

#include <mem.h> // for NULL

unsigned char inportb (unsigned short _port);
void outportb (unsigned short _port, char _data);

#include <asm.h>

#define IO_WAIT() \
		ASM("nop"); ASM("nop"); ASM("nop"); ASM("nop");

#ifdef __cplusplus
}
#endif

#endif /*SYS_H*/
