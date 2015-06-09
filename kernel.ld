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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA. */

OUTPUT_FORMAT("elf32-i386")
ENTRY (start)

__physHigh = 0x01000000;
__physVHigh = 0x80000000;
__physLow   = 0x00100000;
__phys = __physLow;
SECTIONS
{
  . = 1M;

  .text __physLow : AT(__phys) ALIGN(4k) {
    *(.__mbHeader)
    *(.text* .rodata*) /* rodata => read only data: string etc.*/  
    SORT(CONSTRUCTORS)
  
    . = ALIGN(0x4);
    PROVIDE(ctors = .); /* the static objects' constructors array */ /*PROVIDE_HIDDEN?*/
    /* note: must include libgcc in the gcc compilation for it to generate .ctors */
    *(.ctors)    /*should it be KEEP(*(.ctors)) as in http://gcc.gnu.org/bugzilla/show_bug.cgi?id=46770 ?*/
  }
    
  .stab : {
    PROVIDE(__stab_start = .);
    *(.stab);
    PROVIDE(__stab_end = .);
    BYTE(0)		/* force the linker to allocate space for this section */
  }

  .stabstr : {
    PROVIDE(__stabstr_start = .);
    *(.stabstr);
    PROVIDE(__stabstr_end = .);
    BYTE(0)		/* force the linker to allocate space for this section */
  } 
	  
  /* Adjust the address for the data segment to the next page */
  . = ALIGN(0x1000);
  
  .data :
  {
    *(.data)
  }

  .bss :
  {
    *(.bss)
    *(COMMON)
  }
  PROVIDE(__end = .);
}
