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

// implemented from info at http://www.osdever.net/FreeVGA/vga/vga.htm

#ifndef _VGA_H
#define	_VGA_H

#ifdef	__cplusplus
extern "C" {
#endif    

// ______________PORT IO________________

// A - addr        
// D - data
// C - color
// M - mono
// W - write
// R - read
    
// GRAPHICS    
// Graphics registers, address and data ports    
#define VGA_GR_A 0x3CE // r/w
#define VGA_GR_D 0x3CF // r/w

// SEQUENCER    
// sequencer registers, address and data ports    
#define VGA_SEQ_A 0x3C4 // r/w
#define VGA_SEQ_D 0x3C5 // r/w
    
// ATTRIBUTE    
// attribute registers, address and data ports    
#define VGA_ATT_AW 0x3C0 // r/w   
#define VGA_ATT_R 0x3C1 // r/w
    
// CRT    
// crt controller registers, address and data ports    
// color mode
#   define VGA_CRT_AC 0x3D4 // r/w 
#   define VGA_CRT_DC 0x3D5 // r/w
// mono mode
#   define VGA_CRT_AM 0x3B4 // r/w
#   define VGA_CRT_DM 0x3B5 // r/w    

// EXTERNAL REGISTERS    
    
// misc. output register    
#define VGA_MIS_R  0x3CC // read
#define VGA_MIS_W 0x3C2 // write
        
// feature contorl register
#define VGA_FTC_R 0x3CA // read 
// color mode
#   define VGA_FTC_WC 0x3DA // write
// mono mode
#   define VGA_FTC_WM 0x3BA // write
    
// input status #0 register
#define VGA_IS0_R 0x3C2 // read
    
// input status #1 register    
// color mode
#   define VGA_IS1_RC 0x3DA // read
// mono mode
#   define VGA_IS1_RM 0x3B2 // read
    
// COLOR REGISTERS
    
// DAC write address register
#define VGA_DACW_A 0x3C8 // r/w (r not reliable)
    
// DAC read address register
#define VGA_DACR_A  0x3C7 // write

// DAC data register    
#define VGA_DAC_D            0x3C9 // r/w
    
// DAC state register    
#define VGA_DAC_STATE           0x3C7 // read    
    
// ___________SPECIFIC REGISTERS___________

// graphics registers indecis
#define VGA_GR_SET_RESEt_I          0x00
#define VGA_GR_ENABLE_SET_RESEt_I   0x01
#define VGA_GR_COLOR_CMP_I          0x02
#define VGA_GR_DATA_ROTATE_I        0x03
#define VGA_GR_READ_MAP_SELECT_I    0x04
#define VGA_GR_GFX_MODE_I           0x05
#define VGA_GR_MISC_GFX_I           0x06
#define VGA_GR_COLOR_DNT_CARE_I     0x07
#define VGA_GR_BIT_MASK_I           0x08

    
    
#ifdef	__cplusplus
}
#endif

#endif	/* _VGA_H */

