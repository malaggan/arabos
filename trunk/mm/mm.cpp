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
#define FIRST_PAGE_AFTER_KERNEL() kernel_end_addr+(PG_SIZE-kernel_end_addr%PG_SIZE)

// TODO needs to get the size of the physical memory and discard the used frames from the multi-boot info
// now assuming flat, 8MB physical memory
#define ADDRESS_SPACE_PAGES (1024*1024)
int physPages = 8*1024*1024/*mem size*/ / PG_SIZE;
int kernelPages = 4*1024*1024/*kernel hard-coded boot-strap memory*/ / PG_SIZE;
// max pages = 4GB/PG_SIZE = 1048576 pages = (1024*1024) = 1 << 20
// if we gonna make a bit map, we need (1 << 20) / 8 bytes = 131072 = 128 KB of memory = 32 pages
unsigned char freeMem[ADDRESS_SPACE_PAGES] = {0};

PageDirectoryEntry* kernelPageDir;
PageTableEntry* kernelImagePageTable;
PageTableEntry* kernelHeapPageTable;

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

// this function returns the virtual address mapped to the specified physical address in the stack
char* alloc_page()   //****
{
    // look for a free page
    int freePageIndex = -1;
    for(int i = kernelPages/8; i < physPages/8; i ++) // don't look in the kernel pages, cuz they are always non-free
    {
        unsigned char pack = freeMem[i];
        if(pack == 0xFF) // pack full
            continue;

        // found an empty page, look for the first 0-bit
        for(int b = 0; b < 8; b ++)
            if(~(pack & (1<b)))
            {
                freePageIndex = i*8 + b;
                freeMem[i] = freeMem[i] | (1<b); // set as non-free
            }

        if(freePageIndex < 0)
            panic("BUG in alloc_page");
    }
    if(freePageIndex < 0)
    {
        panic("OUT OF MEMORY");
    }

    // now we've got the page index.
    phys_addr_t paddr = freePageIndex*PG_SIZE;

    // now find an empty page in the table (vaddr)
    int emptyPte = -1;
    for(int i = 0; i < 1024; i ++)
    {
        PageTableEntry pte = kernelHeapPageTable[i];
        if(!pte.getPresent())
        {
            emptyPte = i;
            break;
        }
    }
    if(emptyPte < 0)
    {
        panic("no empty PTE although there is an empty physical page !");
    }
    PageTableEntry* pte = &kernelHeapPageTable[emptyPte];
    pte->setPageBase(paddr);
    pte->setPrivilage(Supervisor);
    pte->setAccess(ReadWrite);
    pte->setPresent(true);

    char* vaddr = reinterpret_cast<char*>((1<<22) | (emptyPte<<12));

    invlpg(vaddr);
        
    return vaddr;
}

static void turn_paging_on()
{
    int cr0 = 0;
    rcr0(cr0);
    cr0 |= 0x80000000;
    lcr0(cr0);
}

static void enable_paging()
{
	lcr3(kernelPageDir);

        turn_paging_on();

	printk(LOG "Paging has been just enabled\n");

	// if it was enabled right, then accessing *4MB will cause a page fault
	int cause_it = 0; // 1
	*(char*)(1024*1024*4-1+cause_it) = 0;
}

void init_paging()
{
	kernelPageDir        = reinterpret_cast<PageDirectoryEntry*>(FIRST_PAGE_AFTER_KERNEL());
	kernelImagePageTable = reinterpret_cast<PageTableEntry*>(FIRST_PAGE_AFTER_KERNEL()+  PG_SIZE); // leave a space for the page dir it self
	kernelHeapPageTable  = reinterpret_cast<PageTableEntry*>(FIRST_PAGE_AFTER_KERNEL()+2*PG_SIZE); // leave a space for the previous page table ( a page table takes 1 page frame in memory)

	memset((unsigned char *)kernelPageDir,        0,PG_SIZE);
	memset((unsigned char *)kernelImagePageTable,0,PG_SIZE);
	memset((unsigned char *)kernelHeapPageTable, 0,PG_SIZE);

	// the first page table
        PageDirectoryEntry pde0;
        pde0.setPageTableBase(reinterpret_cast<phys_addr_t>(kernelImagePageTable));
        pde0.setPrivilage(Supervisor);
        pde0.setAccess(ReadWrite);
        pde0.setPresent(true);

        PageDirectoryEntry pde1;
        pde1.setPageTableBase(reinterpret_cast<phys_addr_t>(kernelHeapPageTable));
        pde1.setPrivilage(Supervisor);
        pde1.setAccess(ReadWrite);
        pde1.setPresent(true);
        
        kernelPageDir[0]=pde0;
        kernelPageDir[1]=pde1;

	// we be sure that the previously allocated pages for the page tables are not exceeding 4 MB
	if((FIRST_PAGE_AFTER_KERNEL()+3*PG_SIZE) > 1024*1024*4)
	{
            panic("Kernel and its page tables are taking more than 4 MB");
	}

	// map the first 4 MB ( identity mapping ) 
	for(int i = 0; i < 1024; i++)
	{
            PageTableEntry pte;
            pte.setPageBase(i<<12); // the physical address
            pte.setPrivilage(Supervisor);
            pte.setAccess(ReadWrite);
            pte.setPresent(true);

            kernelImagePageTable[i] = pte;
	}

        // set all pages as free
        for(int i = 0; i < ADDRESS_SPACE_PAGES/8 /*or can be physPages instead*/; i++)
            freeMem[i] = 0;
        // mark the first 4MB as non-free
        for(int i = 0; i < 1024/8; i++)
            freeMem[i] = 0xFF;
        
	enable_paging();

#if 1
        // test
        for(int p = 0; p < 4; p ++) // we only have 1024 pages left,
        {
            char* newPage = alloc_page();
            printf("allocated 0x%x\n",newPage);
            for(int i = 0; i < 1024; i ++)
                newPage[i] = 3;
        }
#endif

}

#ifdef __cplusplus
}
#endif