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

#include <multiboot.h>
#include <printf.h>
#include <console.h>
#include <mem.h>
#include <gdt.h>
#include <idt.h>
#include <timer.h>
#include <kb.h>
#include <mm.h>
#include <cpptest.h>
/* Macros. */

/* Check if the bit BIT in FLAGS is set. */
#define CHECK_FLAG(flags,bit)	((flags) & (1 << (bit)))

extern int __end,__phys;
int kernel_end_addr=(int)(&__end),kernel_load_addr=(int)(&__phys); // the linker symbol have only an address (in the symbol table)

/* Forward declarations. */
void cmain (unsigned long magic, unsigned long addr);

void mem_check(unsigned long magic,unsigned long addr)
{
	multiboot_info_t *mbi;

	/* Clear the screen. */
	cls ();

	/* Am I booted by a Multiboot-compliant boot loader? */
	if (magic != MULTIBOOT_BOOTLOADER_MAGIC)
	{
		printf ("Invalid magic number: 0x%x\n", (unsigned) magic);
		return;
	}

	/* Set MBI to the address of the Multiboot information structure. */
	mbi = (multiboot_info_t *) addr;

	/* Print out the flags. */
	printf ("flags = 0x%x\n", (unsigned) mbi->flags);

	/* Are mem_* valid? */
	if (CHECK_FLAG (mbi->flags, 0))
		printf ("mem_lower = %uKB, mem_upper = %uKB\n",
		(unsigned) mbi->mem_lower, (unsigned) mbi->mem_upper);

	/* Is boot_device valid? */
	if (CHECK_FLAG (mbi->flags, 1))
		printf ("boot_device = 0x%x\n", (unsigned) mbi->boot_device);

	/* Is the command line passed? */
	if (CHECK_FLAG (mbi->flags, 2))
		printf ("cmdline = %s\n", (char *) mbi->cmdline);

	/* Are mods_* valid? */
	if (CHECK_FLAG (mbi->flags, 3))
	{
		module_t *mod;
		int i;

		printf ("mods_count = %d, mods_addr = 0x%x\n",
			(int) mbi->mods_count, (int) mbi->mods_addr);
		for (i = 0, mod = (module_t *) mbi->mods_addr;
			i < mbi->mods_count;
			i++, mod += sizeof (module_t))
			printf (" mod_start = 0x%x, mod_end = 0x%x, string = %s\n",
			(unsigned) mod->mod_start,
			(unsigned) mod->mod_end,
			(char *) mod->string);
	}

	/* Bits 4 and 5 are mutually exclusive! */
	if (CHECK_FLAG (mbi->flags, 4) && CHECK_FLAG (mbi->flags, 5))
	{
		printf ("Both bits 4 and 5 are set.\n");
		return;
	}

	/* Is the symbol table of a.out valid? */
	if (CHECK_FLAG (mbi->flags, 4))
	{
		aout_symbol_table_t *aout_sym = &(mbi->u.aout_sym);

		printf ("aout_symbol_table: tabsize = 0x%0x, "
			"strsize = 0x%x, addr = 0x%x\n",
			(unsigned) aout_sym->tabsize,
			(unsigned) aout_sym->strsize,
			(unsigned) aout_sym->addr);
	}

	/* Is the section header table of ELF valid? */
	if (CHECK_FLAG (mbi->flags, 5))
	{
		elf_section_header_table_t *elf_sec = &(mbi->u.elf_sec);

		printf ("elf_sec: num = %u, size = 0x%x,"
			" addr = 0x%x, shndx = 0x%x\n",
			(unsigned) elf_sec->num, (unsigned) elf_sec->size,
			(unsigned) elf_sec->addr, (unsigned) elf_sec->shndx);
	}

	/* Are mmap_* valid? */
	if (CHECK_FLAG (mbi->flags, 6))
	{
		memory_map_t *mmap;

		printf ("mmap_addr = 0x%x, mmap_length = 0x%x\n",
			(unsigned) mbi->mmap_addr, (unsigned) mbi->mmap_length);
		for (mmap = (memory_map_t *) mbi->mmap_addr;
			(unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
			mmap = (memory_map_t *) ((unsigned long) mmap
			+ mmap->size + sizeof (mmap->size)))
			printf (" size = 0x%x, base_addr = 0x%x%x,"
			" length = 0x%x%x, type = 0x%x\n",
			(unsigned) mmap->size,
			(unsigned) mmap->base_addr_high,
			(unsigned) mmap->base_addr_low,
			(unsigned) mmap->length_high,
			(unsigned) mmap->length_low,
			(unsigned) mmap->type);
	}
}

/* Check if MAGIC is valid and print the Multiboot information structure
pointed by ADDR. */
void
cmain (unsigned long magic, unsigned long addr)
{
	init_video();
	int memCheck = 1;
	if(memCheck)
	{
		mem_check(magic,addr);		
	}


	printf(WHITE "Welcome to " REDB "ArOS" BLUEB " v 0.003_1!\n" NORMAL);	
	SHOW_STAT_OK("Boot");

	gdt_install();
	SHOW_STAT_OK("GDT");

	idt_install();
	//SHOW_STAT_OK("IDT");	
	irq_install();
	//SHOW_STAT_OK("IRQ");	
	timer_install();
	ASM ("sti");
	SHOW_STAT_OK("IDT");

	init_kb();

	init_paging();
        
        SHOW_STAT_FAILED("Kernel memory allocator");
	SHOW_STAT_FAILED("Process manager");
	SHOW_STAT_FAILED("Discovering devices");
	SHOW_STAT_FAILED("Filesystem");
	SHOW_STAT_FAILED("Networking");
	SHOW_STAT_FAILED("Shell");
								
	//printf("Kernel size is %d bytes (%d KB) [end= %x,load= %x\n",(kernel__end_addr-kernel_load_addr),
	//											(kernel__end_addr-kernel_load_addr)>>10,
	//											kernel__end_addr,kernel_load_addr);
        cppmain();
        
	unsigned long i = 0;
	for(; i <  500000; i++);
	panic("panic test...");
}