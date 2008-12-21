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

#include <mm.h>
#include <lib.h>
#include <asm.h>

extern int _end,_phys;
extern int kernel_end_addr;

#define PG_SIZE (1024*4)	 // 4 KB

// the first page-boundary address after the kernel
#define MAKE_KERNEL_PAGE_DIR() kernel_end_addr+(PG_SIZE-kernel_end_addr%PG_SIZE) 

page_directory_t kernel_page_dir;
page_table_t kernel_page_table;
page_table_t free_page_stack_table;

// this contains physical addresses
struct free_page_list 
{
	page_table_entry* tos;   
} page_stack;

// this function takes a virtual address, get its physical address and adds it back in he list
void free_page(page_table_entry page)   //****
{
	// don't forget to call INVLPG to flush the presenet bit
	// don't forget to check for overflows and undetflows
	*(page_stack.tos--) = /*get the physical address of the vaddr*/
                               *(   (page_table_entry*)  
                                            (
                                                    (page_table_t)
                                                            (kernel_page_dir[	((int)page & 0xFFC00000)	>>22	])
                                            )[	((int)page & 0x003FF000)	>>12	]
                                    );

}

// this function returns the virtual address mapped to the specified physical address in the stack
char* alloc_page()   //****
{
	// don't forget to call INVLPG to flush the presenet bit
	// don't forget to check for overflows and undetflows
		   // phys addr = *(page_stack.tos++)
		   // we must map first then return the virtual addr
	return NULL;
}


void init_paging()
{
	kernel_page_dir = (page_directory_t)MAKE_KERNEL_PAGE_DIR();
	kernel_page_table = (page_table_t)(MAKE_KERNEL_PAGE_DIR()+PG_SIZE); // leave a space for the page dir it self
	free_page_stack_table = (page_table_t)(MAKE_KERNEL_PAGE_DIR()+2*PG_SIZE); // leave a space for the previous page table ( a page table takes 1 page frame in memory) 

	memset((unsigned char *)kernel_page_dir,0,PG_SIZE);
	memset((unsigned char *)kernel_page_table,0,PG_SIZE);
	memset((unsigned char *)free_page_stack_table,0,PG_SIZE);

	// the first page table
	kernel_page_dir[0] = (page_table_t)((int)(kernel_page_table)|3); // 011, supervisor,read/write/present
	kernel_page_dir[1] = (page_table_t)((int)(free_page_stack_table)|3); // 011, supervisor,read/write/present

	// we be sure that the previously allocated pages for the page tables are not exceeding 4 MB
	if((page_table_t)(MAKE_KERNEL_PAGE_DIR()+3*PG_SIZE) > 1024*1024*4)
	{
		panic("Kernel and its page tables are taking more than 4 MB");
	}

	// map the first 4 MB ( identity mapping ) 
	for(int i = 0; i < 1024; i++)
	{
		kernel_page_table[i] = (i << 12)|3;	
	}	

	// we have mapped only one page table for the kernel.
	// a stack to follow the free pages will take upto 1 M pages * 4 = 4 MB
	// = anothe page table.

	// create free page stack ( from 4 MB to 8 MB physical mem)
	for(int i = 0; i < 1024; i++)
	{
		free_page_stack_table[i] = ((i+1024) << 12)|3;
	}
	page_stack.tos = (page_table_entry*)(1024*1024*8-sizeof(page_table_entry)); // 8 MB. we subtract that value so we don't exceed the boundary

	// ******** now push the free pages, consider the reserved memory aread specified by GRUB to be not free.
	//*****************************************>>>

	enable_paging();	
}

void turn_paging_on()
{
    int cr0 = 0;
    rcr0(cr0);
    cr0 |= 0x80000000;
    lcr0(cr0);
}

void enable_paging()
{
	lcr3(kernel_page_dir);
                
        turn_paging_on();
        
	printk(LOG "Paging has been just enabled\n");

	// if it was enabled right, then accessing *8MB will cause a page fault
	int cause_it = 0; // 1
	*(char*)(1024*1024*8-1+cause_it) = 0;
}
