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
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

OUTPUT_FORMAT("elf32-i386")
ENTRY (start)

__physHigh = 0x01000000;
__physVHigh = 0x80000000;
__physLow   = 0x00100000;
__phys = __physLow;
SECTIONS
{
  .text __physLow : AT(__phys) {
    code = .;
    *(.text)
    . = ALIGN(4096);
  }
  .data : AT(__phys + (data - code))
  {
    data = .;
    *(.data)
    __end_data = .;
    . = ALIGN(4096);
  }
  .bss : AT(__phys + (bss - code))
  {
    bss = .;
    *(.bss)
    *(COMMON)
    __end_bss = .;
    . = ALIGN(4096);
  }
  __end = .;
}
