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

#include "timer.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int _end,_phys; // filled by the linker script
extern int kernel_end_addr; // calculated in booting

#define PG_SIZE (1024*4)	 // 4 KB

// the first page-boundary address after the kernel
#define FIRST_AFTER_KERNEL_PAGE_ADDR() kernel_end_addr+(PG_SIZE-kernel_end_addr%PG_SIZE)

PageDirectoryEntry* kernel_page_dir;
PageTableEntry* kernel_image_page_table;
PageTableEntry* kernel_heap_page_table;
int first_free_frame = NULL;
int last_free_frame = NULL;

//phys_addr_t page_table_of(linear_addr_t virt)
//{
//    int page_table_index = ((int)virt.value & 0xFFC00000) >> 22;
//    return ((page_directory_entry*)(kernel_page_dir + sizeof(page_directory_entry) * page_table_index))->pageTableBase;
//}
//
//phys_addr_t page_base_of(linear_addr_t virt)
//{
//    int page_index = ((int)virt.value & 0x003FF000) >> 12;
//    return ((page_table_entry*)(page_table_of(virt) + sizeof(page_table_entry) * page_index))->pageBase;
//}
//
//phys_addr_t virt2phys(linear_addr_t virt)
//{
//    return page_base_of(virt);
//}
//
//// this function takes a virtual address, get its physical address and adds it back in the list
//void unmap_page(int* virt)   //****
//{
//    // don't forget to check for overflows and undetflows
//
//    // this function unmaps a binding between a physical address and a virtual address,
//    // adds the physical address to free list,and adds the virtual address to free list
//    // and clears the present bit in the corresponding page table entry
//
//    // update free phys list
//    // TODO check if last_free_frame = null
//    linear_addr_t addr;
//    addr.value = (unsigned long)virt;
//    int phys = virt2phys(addr);
//    *(int*)last_free_frame = phys;
//    last_free_frame = phys;
//
//    // clear the present bit in the page table, then call INVLPG to flush it
//    page_base_of(addr); // << clear the present bit in that ! ezay ma3rafsh !
//}
//
//// this function returns the virtual address mapped to the specified physical address in the stack
//char* alloc_page()   //****
//{
//    // don't forget to call INVLPG to flush the presenet bit
//    // don't forget to check for overflows and undetflows
//        // phys addr = *(page_stack.tos++)
//        // we must map first then return the virtual addr
//	return NULL;
//}


void init_paging()
{
	kernel_page_dir         = reinterpret_cast<PageDirectoryEntry*>(FIRST_AFTER_KERNEL_PAGE_ADDR());
	kernel_image_page_table = reinterpret_cast<PageTableEntry*>(FIRST_AFTER_KERNEL_PAGE_ADDR()+  PG_SIZE); // leave a space for the page dir it self
	kernel_heap_page_table  = reinterpret_cast<PageTableEntry*>(FIRST_AFTER_KERNEL_PAGE_ADDR()+2*PG_SIZE); // leave a space for the previous page table ( a page table takes 1 page frame in memory)

	memset((unsigned char *)kernel_page_dir,        0,PG_SIZE);
	memset((unsigned char *)kernel_image_page_table,0,PG_SIZE);
	memset((unsigned char *)kernel_heap_page_table, 0,PG_SIZE);

	// the first page table
        PageDirectoryEntry pde0;
        pde0.setPageTableBase(reinterpret_cast<phys_addr_t>(kernel_image_page_table));
        pde0.setPrivilage(Supervisor);
        pde0.setAccess(ReadWrite);
        pde0.setPresent(true);

        PageDirectoryEntry pde1;
        pde1.setPageTableBase(reinterpret_cast<phys_addr_t>(kernel_heap_page_table));
        pde1.setPrivilage(Supervisor);
        pde1.setAccess(ReadWrite);
        pde1.setPresent(true);
        
        kernel_page_dir[0]=pde0;
        kernel_page_dir[1]=pde1;

	// we be sure that the previously allocated pages for the page tables are not exceeding 4 MB
	if((FIRST_AFTER_KERNEL_PAGE_ADDR()+3*PG_SIZE) > 1024*1024*4)
	{
            panic("Kernel and its page tables are taking more than 4 MB");
	}

	// map the first 4 MB ( identity mapping ) 
	for(int i = 0; i < 1024; i++)
	{
            PageTableEntry pte2;
            pte2.setPageBase(i<<12);
            pte2.setPrivilage(Supervisor);
            pte2.setAccess(ReadWrite);
            pte2.setPresent(true);

            kernel_image_page_table[i] = pte2;

/*
            if(i > 0) // fill the free page chain
                *(int*)((1 << 22) + ((i-1) << 12) ) = ((1 << 22) + ((i) << 12));
*/
	}

        first_free_frame = ((1 << 22));
        last_free_frame = ((1 << 22) + ((1023) << 12));

	// we have mapped only one page table for the kernel.
	// a stack to follow the free pages will take upto 1 M pages * 4 = 4 MB
	// = anothe page table.

	// create free page stack ( from 4 MB to 8 MB physical mem)
/*
	for(int i = 0; i < 1024; i++)
	{
            page_table_entry entry;
            entry.pageBase = 1 << 22 + i << 12;
            entry.data.present = 1;
            entry.data.readWrite = 1;
            entry.data.userSupervisor = 0;

            *((page_table_entry*)(kernel_heap_page_table + sizeof(page_table_entry) * i)) = entry;
	}
*/

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
	*(char*)(1024*1024*4-1+cause_it) = 0;
}

#ifdef __cplusplus
}
#endif