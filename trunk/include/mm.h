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

#ifndef MM_H
#define MM_H

// page table entry format :
// addr/avail to os/zero/zero/dirty/accessed/page cache disable/pg write throuhg(1) back(0)/user,supervisor/write,read/present

// page  dir entry
// addr/avail to os/zero/page size/zero/accessed/page cache disable/pg write throuhg(1) back(0)/user,supervisor/write,read/present

typedef long page_table_entry; // (physical address with flags)	 // can it be a bit-fielded struct ?
typedef page_table_entry* /*[1024]*/ page_table_t;
typedef page_table_t* /*[1024]*/ page_directory_t;

extern page_directory_t kernel_page_dir;

void init_paging();
inline void enable_paging();

#endif // MM_H 
