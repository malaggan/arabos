#include <gdt.h>

/* Our GDT, with 3 entries, and finally our special GDT pointer */
struct gdt_entry gdt[3] = { {0,0,0,0,0,0}, };
struct tag_gdt_ptr gdt_ptr = { 0 , 0 };

/* Setup a descriptor in the Global Descriptor Table */
void gdt_set_gate(int num, unsigned long base, unsigned long limit, unsigned char access, unsigned char gran)
{
	/* Setup the descriptor base address */
	gdt[num].base_low = (base & 0xFFFF);
	gdt[num].base_middle = (base >> 16) & 0xFF;
	gdt[num].base_high = (base >> 24) & 0xFF;

	/* Setup the descriptor limits */
	gdt[num].limit_low = (limit & 0xFFFF);
	gdt[num].granularity = ((limit >> 16) & 0x0F);

	/* Finally, set up the granularity and access flags */
	gdt[num].granularity |= (gran & 0xF0);
	gdt[num].access = access;
}

/* Should be called by main. This will setup the special GDT
*	pointer, set up the first 3 entries in our GDT, and then
*	finally call gdt_flush() in our assembler file in order
*	to tell the processor where the new GDT is and update the
*	new segment registers */
void gdt_install()
{
	/* Setup the GDT pointer and limit */
	gdt_ptr.limit = (sizeof(struct gdt_entry) * 3) - 1;
	gdt_ptr.base = (unsigned int)&gdt;

	/* Our NULL descriptor */
	gdt_set_gate(0, 0, 0, (unsigned char)0, (unsigned char)0);

	/* The second entry is our Code Segment. The base address
	*	is 0, the limit is 4GBytes, it uses 4KByte granularity,
	*	uses 32-bit opcodes, and is a Code Segment descriptor.
	*	Please check the table above in the tutorial in order
	*	to see exactly what each value means */
	gdt_set_gate(1,
		/*0x80100000*/0, /*base*/
		0xFFFFFFFF, /*limit*/
		GDT_KERNEL_CODE_FLAGS,
		KERNEL_GRAN);

	/* The third entry is our Data Segment. It's EXACTLY the
	*	same as our code segment, but the descriptor type in
	*	this entry's access byte says it's a Data Segment */
	gdt_set_gate(2,
		/*0x80100000*/0,
		0xFFFFFFFF,
		GDT_KERNEL_DATA_FLAGS,
		KERNEL_GRAN);

	/* Flush out the old GDT and install the new changes! */
	gdt_flush();
}
