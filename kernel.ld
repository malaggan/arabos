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
