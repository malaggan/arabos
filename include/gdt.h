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

#ifndef GDT_H
#define GDT_H

#ifdef __cplusplus
extern "C" {
#endif

#define KERNEL_CODE_SELECTOR 0x08
#define KERNEL_DATA_SELECTOR 0x10

#define GDT_PRESENT			(0x80)
#define GDT_RING(n)			((n&0x3)<<5)

#define GDT_EXECUTABLE		(0x8)
//#define GDT_NON_EXECUTABLE	(0x0)

#define GDT_CONFORMING		(0x4)
//#define GDT_NON_CONFORMING	(0x0)

#define GDT_WRITABLE		(0x2)
//#define GDT_READ_ONLY		(0x0)

#define GDT_ACCESSED		(0x1)

#define GRAN_4K_PAGES		(0x80)
#define GRAN_OP_SIZE_32		(0x40)

//0xCF /*1 gran: 4k page, 1 16/32 : 32, 00xxxx*/
#define KERNEL_GRAN			(unsigned char)(GRAN_4K_PAGES|GRAN_OP_SIZE_32|0xF)

//0x9A /*1 present,	00 ring, 1 fixed val, 1 executable, 0 Expansion direction/ conforming, 1 Writable/ readable, 1 accessed*/,
#define GDT_KERNEL_CODE_FLAGS (unsigned char)( 0x10 | GDT_PRESENT | GDT_RING( 0 ) | GDT_EXECUTABLE	| GDT_WRITABLE | GDT_ACCESSED )

//0x92 /*1 present,	00 ring, 1 fixed val, 0 executable, 0 Expansion direction/ conforming, 1 Writable/ readable, 0 accessed*/,
#define GDT_KERNEL_DATA_FLAGS (unsigned char)( 0x10 | GDT_PRESENT | GDT_RING( 0 ) | GDT_WRITABLE )


/* Defines a GDT entry. We say packed, because it prevents the
*	compiler from doing things that it thinks is best: Prevent
*	compiler "optimization" by packing */
struct gdt_entry
{ /*see http://www.osdever.net/tutorials/pm.php?the_id=16 for description*/
	unsigned short limit_low;
	unsigned short base_low;
	unsigned char base_middle;
	unsigned char access;
	unsigned char granularity;
	unsigned char base_high;
} __attribute__((packed));

/* Special pointer which includes the limit: The max bytes
*	taken up by the GDT, minus 1. Again, this NEEDS to be packed */
struct tag_gdt_ptr/*the address of this struct is the one which is passed to the lgdt instuction*/
{
	unsigned short limit;
	unsigned int base;
} __attribute__((packed));

/* This will be a function in start.asm. We use this to properly
*	reload the new segment registers */
extern void gdt_flush();

/* Setup a descriptor in the Global Descriptor Table */
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran);
void gdt_install();

#ifdef __cplusplus
}
#endif

#endif // GDT_H
