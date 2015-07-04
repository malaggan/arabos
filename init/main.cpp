#include <console.h>
#include <serial.h>
#include <panic.h>

/* Check if MAGIC is valid and print the Multiboot information structure pointed by ADDR. */
void mem_check(unsigned long magic, unsigned long addr);

void cppmain();

extern "C" void cmain (unsigned long magic, unsigned long addr)
{
	init_serial();
	init_video();

	mem_check(magic,addr);

	cppmain();

	panic("exiting cmain\n");
}
