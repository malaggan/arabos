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

int SET_BIT(const int x, const int b) {return (x|(1<<b)); }
int CLR_BIT(const int x, const int b) {return (x&~(1<<b)); }
int GET_BIT(const int x, const int b) {return (x&(1<<b))>0?1:0; }
int SET_BIT(const int x, const int b, const int v) {return (v?SET_BIT(x,b):CLR_BIT(x,b)); }

PageTableEntry::PageTableEntry():value(0){}
PageTableEntry::PageTableEntry(unsigned long aValue):value(aValue){}

unsigned long PageTableEntry::getValue() const {return value;}

phys_addr_t PageTableEntry::getPageBase() const { return value & ((~0)<<12); }
void PageTableEntry::setPageBase(phys_addr_t pageBase) { value = (value & ~((~0)<<12)) | (pageBase & ((~0)<<12)); }

unsigned int PageTableEntry::getExtra() const { return (value >> 9) & 0x7;}
void PageTableEntry::setExtra(unsigned int extra) { value = (value & ((~0)<<9)) | (extra & ((~0)>>3)); }

bool PageTableEntry::getGlobal() const { return GET_BIT(value,8); }
void PageTableEntry::setGlobal(bool global) { value=SET_BIT(value,8,global); }

bool PageTableEntry::getPat() const { return GET_BIT(value,7); }
void PageTableEntry::setPat(bool pat) { value=SET_BIT(value,7,pat); }

unsigned int PageTableEntry::getDirty() const { return GET_BIT(value,6); }
unsigned int PageTableEntry::getAccessed() const { return GET_BIT(value,5); }

bool PageTableEntry::getCahceEnabled() const { return !GET_BIT(value,4); }
void PageTableEntry::setCahceEnabled(bool enabled) { value=SET_BIT(value,4,!enabled); }

CacheMode PageTableEntry::getCacheMode() const { return static_cast<CacheMode>(GET_BIT(value,3)); }
void PageTableEntry::setCacheMode(CacheMode mode) { value=SET_BIT(value,3,mode); }

Privilage PageTableEntry::getPrivilage() const { return static_cast<Privilage>(GET_BIT(value,2)); }
void PageTableEntry::setPrivilage(Privilage mode) { value=SET_BIT(value,2,mode); }

Access PageTableEntry::getAccess() const { return static_cast<Access>(GET_BIT(value,1)); }
void PageTableEntry::setAccess(Access mode) { value=SET_BIT(value,1,mode); }

bool PageTableEntry::getPresent() const { return GET_BIT(value,0); }
void PageTableEntry::setPresent(bool present) { if(!present) value=0; value=SET_BIT(value,0,present); }

void PageTableEntry::print() const
{
	printk("PTE PB:0x%x, EX:0x%x, G:%d, PAT:%d, D:%d, A:%d, PCD:%d, PWT:%d, US:%d, RW:%d, P:%d\n",getPageBase(),getExtra(),getGlobal(),
	       getPat(), getDirty(), getAccessed(), !getCahceEnabled(), getCacheMode(), getPrivilage(), getAccess(),getPresent() );
}

PageDirectoryEntry::PageDirectoryEntry():value(0){}
PageDirectoryEntry::PageDirectoryEntry(unsigned long aValue):value(aValue){}

unsigned long PageDirectoryEntry::getValue() const {return value;}

phys_addr_t PageDirectoryEntry::getPageTableBase() const { return value & ((~0)<<12); }
void PageDirectoryEntry::setPageTableBase(phys_addr_t pageTableBase) { value = (value & ~((~0)<<12)) | (pageTableBase & ((~0)<<12)); }

unsigned int PageDirectoryEntry::getExtra() const { return (value >> 9) & 0x7;}
void PageDirectoryEntry::setExtra(unsigned int extra) { value = (value & ((~0)<<9)) | (extra & ((~0)>>3)); }

bool PageDirectoryEntry::getGlobal() const { return GET_BIT(value,8); } // ignored
void PageDirectoryEntry::setGlobal(bool global) { value=SET_BIT(value,8,global); }

PageSize PageDirectoryEntry::getPageSize() const { return static_cast<PageSize>(GET_BIT(value,7)); }
void PageDirectoryEntry::setPageSize(PageSize pageSize) { value=SET_BIT(value,7,pageSize); }

//bit 6 reserved

unsigned int PageDirectoryEntry::getAccessed() const { return GET_BIT(value,5); }

bool PageDirectoryEntry::getCahceEnabled() const { return !GET_BIT(value,4); }
void PageDirectoryEntry::setCahceEnabled(bool enabled) { value=SET_BIT(value,4,!enabled); }

CacheMode PageDirectoryEntry::getCacheMode() const { return static_cast<CacheMode>(GET_BIT(value,3)); }
void PageDirectoryEntry::setCacheMode(CacheMode mode) { value=SET_BIT(value,3,mode); }

Privilage PageDirectoryEntry::getPrivilage() const { return static_cast<Privilage>(GET_BIT(value,2)); }
void PageDirectoryEntry::setPrivilage(Privilage mode) { value=SET_BIT(value,2,mode); }

Access PageDirectoryEntry::getAccess() const { return static_cast<Access>(GET_BIT(value,1)); }
void PageDirectoryEntry::setAccess(Access mode) { value=SET_BIT(value,1,mode); }

bool PageDirectoryEntry::getPresent() const { return GET_BIT(value,0); }
void PageDirectoryEntry::setPresent(bool present) { if(!present) value=0; value=SET_BIT(value,0,present); }

void PageDirectoryEntry::print() const
{
	printk("PTE PB:0x%x, EX:0x%x, G:%d, PS:%d, A:%d, PCD:%d, PWT:%d, US:%d, RW:%d, P:%d\n",getPageTableBase(),getExtra(),getGlobal(),
	       getPageSize(), getAccessed(), !getCahceEnabled(), getCacheMode(), getPrivilage(), getAccess(),getPresent() );
}

extern "C" {

	extern int _end,_phys; // filled by the linker script
	extern int kernel_end_addr; // calculated in booting

#define PG_SIZE (1024*4)     // 4 KB

// the first page-boundary address after the kernel
#define FIRST_PAGE_AFTER_KERNEL() kernel_end_addr+(PG_SIZE-kernel_end_addr%PG_SIZE)

// TODO needs to get the size of the physical memory and discard the used frames from the multi-boot info
// now assuming flat, 32MB physical memory
#define MEM_IN_BYTES (8*1024*1024)
	int physPages = MEM_IN_BYTES / PG_SIZE;
	int kernelPages = 4*1024*1024 / PG_SIZE; /*kernel hard-coded boot-strap memory: $ MB*/

// max pages = 4GB/PG_SIZE = 1048576 pages = (1024*1024) = 1 << 20
// if we gonna make a bit map, we need (1 << 20) / 8 bytes = 131072 = 128 KB of memory = 32 pages
	unsigned char freeMem[MEM_IN_BYTES/8] = {0};

	PageDirectoryEntry* kernelPageDir;
	PageTableEntry* kernelHeapPageTable;

	void free_page(char* pageBase)
	{
		int vaddr = (int)pageBase;
		int ixPde = (vaddr>>22) & 0x3FF;
		int ixPte = (vaddr>>12) & 0x3FF; // 10 bits

		PageDirectoryEntry* pde = &kernelPageDir[ixPde];
		PageTableEntry* pte = &(reinterpret_cast<PageTableEntry*>(pde->getPageTableBase())[ixPte]);
		phys_addr_t paddr = pte->getPageBase();
		printk(TRACE "freeing %d:%d:%d @ v:0x%x p:0x%x\n",ixPde,ixPte,vaddr&0xFFF, pageBase, paddr);

		int frameIndex = paddr/PG_SIZE;
		int byteIndex = frameIndex / 8;
		int bitIndex = frameIndex % 8;

		freeMem[byteIndex] = freeMem[byteIndex] & ~(1<<bitIndex);
		pte->setPresent(false);
	}

// this function returns the virtual address mapped to the specified physical address in the stack
	char* alloc_page()   //****
	{
		// look for a free page
		int freePageIndex = -1;
		for(int i = kernelPages/8; i < physPages/8; i ++) // don't look in the kernel pages, cuz they are always non-free
		{
			unsigned char pack = freeMem[i];
			if(0xFF == pack) // pack full
				continue;

			// found an empty page, look for the first 0-bit
			for(int b = 0; b < 8; b ++)
				if(!(pack & (1<<b)))
				{
					freePageIndex = i*8 + b;
					freeMem[i] = freeMem[i] | (1<<b); // set as non-free
					break;
				}

			if(freePageIndex < 0)
				panic("BUG in alloc_page");

			break;
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

		printk(TRACE "alloc_page : vaddr(0x%x)->paddr(0x%x) frameIndex(%d)\n",vaddr,paddr,freePageIndex);

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
		kernelPageDir    = reinterpret_cast<PageDirectoryEntry*>(FIRST_PAGE_AFTER_KERNEL());
		PageTableEntry* kernelImagePageTable = reinterpret_cast<PageTableEntry*>(FIRST_PAGE_AFTER_KERNEL()+  PG_SIZE); // leave a space for the page dir it self
		kernelHeapPageTable  = reinterpret_cast<PageTableEntry*>(FIRST_PAGE_AFTER_KERNEL()+2*PG_SIZE); // leave a space for the previous page table ( a page table takes 1 page frame in memory)

		memset((unsigned char *)kernelPageDir,    0,PG_SIZE);
		memset((unsigned char *)kernelImagePageTable, 0,PG_SIZE);
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
		for(int i = 1; i < 1024; i++) // NOTE: i = 1, not i = 0, so as not to map PAGE 0 to disallow null pointers
		{
			PageTableEntry pte;
			pte.setPageBase(i<<12); // the physical address
			pte.setPrivilage(Supervisor);
			pte.setAccess(ReadWrite);
			pte.setPresent(true);

			kernelImagePageTable[i] = pte;
		}


		// set all pages as free
		for(int i = 0; i < MEM_IN_BYTES/8 ; i++)
			freeMem[i] = 0;
		// mark the first 4MB (1024 pages) as non-free
		for(int i = 0; i < 1024/8; i++)
			freeMem[i] = 0xFF;

		enable_paging();

#if 0
		// test
		//for(int p = 0; p < 4; p ++) // we only have 1024 pages left,
		{
			char* newPage1 = alloc_page();
			printf("allocated 0x%x\n",newPage1);
			for(int i = 0; i < 1024; i ++)
				newPage1[i] = 3;

			char* newPage2 = alloc_page();
			printf("allocated 0x%x\n",newPage2);
			for(int i = 0; i < 1024; i ++)
				newPage2[i] = 3;

			char* newPage3 = alloc_page();
			printf("allocated 0x%x\n",newPage3);
			for(int i = 0; i < 1024; i ++)
				newPage3[i] = 3;

			char* newPage4 = alloc_page();
			printf("allocated 0x%x\n",newPage4);
			for(int i = 0; i < 1024; i ++)
				newPage4[i] = 3;

			free_page(newPage3);
			free_page(newPage4);

			newPage3 = alloc_page();
			printf("allocated 0x%x\n",newPage3);
			for(int i = 0; i < 1024; i ++)
				newPage3[i] = 3;

			free_page(newPage1);
			free_page(newPage2);
		}
#endif
	}

} // extern "C"
