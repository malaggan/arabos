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
  }

  .ctors : {
    . = ALIGN(0x4);	
    /* gcc uses crtbegin.o to find the start of the constructors, so we make sure it is first.
    Because this is a wildcard, it doesn't matter if the user does not actually link against 
    crtbegin.o; the linker won't look for a file to match a wildcard.  The wildcard also means 
    that it doesn't matter which directory crtbegin.o is in.  */
    KEEP (*crtbegin*.o(.ctors))
    /* We don't want to include the .ctor section from from the crtend.o file until after the 
    sorted ctors. The .ctor section from the crtend file contains the end of ctors marker and 
    it must be last */
    KEEP (*(EXCLUDE_FILE (*crtend*.o ) .ctors))
    KEEP (*(SORT(.ctors.*)))
    KEEP (*(.ctors))
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
