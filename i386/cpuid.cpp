#include <asm.h>
#include <printf.h>
#include <printk.h>

bool cpuid_available()
{
	// cpuid is available if the ID bit in EFLAGS can be changed

	// read eflags
	int original_flags;
	ASM("pushf\n"
	    "pop %0" : "=m"(original_flags));

	constexpr int CPUID_BIT = 21;

	// flip the ID bit
	int modified_flags = (original_flags & (1<<CPUID_BIT))
		? (original_flags & ~(1<<CPUID_BIT))
		: (original_flags | (1<<CPUID_BIT));

	int read_flags;

	// write then read then restore original
	ASM("push %1\n popf  \n"
	    "pushf  \n pop %0\n"
	    "push %2\n popf  \n"
	    : "=m"(read_flags)
	    : "m"(modified_flags), "m"(original_flags));

	if(read_flags == original_flags || read_flags != modified_flags)
		return false;
	return true;
}

void vendor_string()
{
	int _ebx,_ecx,_edx;

	ASM ("cpuid"
	     :"=b" (_ebx),
	      "=c" (_ecx),
	      "=d" (_edx)
	     : "a"(0));

	printk(LOG "CPU vendor string: %c%c%c%c%c%c%c%c%c%c%c%c\n",
	       (char)((_ebx&0x000000ff)>>0),
	       (char)((_ebx&0x0000ff00)>>8),
	       (char)((_ebx&0x00ff0000)>>16),
	       (char)((_ebx&0xff000000)>>24),

	       (char)((_edx&0x000000ff)>>0),
	       (char)((_edx&0x0000ff00)>>8),
	       (char)((_edx&0x00ff0000)>>16),
	       (char)((_edx&0xff000000)>>24),

	       (char)((_ecx&0x000000ff)>>0),
	       (char)((_ecx&0x0000ff00)>>8),
	       (char)((_ecx&0x00ff0000)>>16),
	       (char)((_ecx&0xff000000)>>24)
		);
}

void cpuid_check()
{
	if(!cpuid_available())
	{
		printk(LOG "CPUID instruction is not available on this CPU (couldn't flip the ID bit in EFLAGS)\n");
		return;
	}

	vendor_string();

	// cpu features:

	static char* edx_features[] = {
		"FPU (x87 FPU on Chip)",
		"VME (Virtual-8086 Mode Enhancement)",
		"DE (Debugging Extensiosn)",
		"PSE (Page Size Extensions)",
		"TSC (Time Stamp Counter)",
		"MSR (RDMSR and WRMSR Support)",
		"PAE (Physical Address Extensions)",
		"MCE (Machine Check Exception)",
		"CX8 (CMPXCHG8B)",
		"APIC (APIC on Chip)",
		"-",
		"SEP (SYSENTER and SYSEXIT)",
		"MTRR (Memory Type Range Registers)",
		"PGE (PTE Global Bit)",
		"MCA (Machine Check Architecture)",
		"CMOV (Conditional Move/Compare Instruction)",
		"PAT (Page Attribute Table)",
		"PSE-36 (Page Size Extension)",
		"PSN (Processor Serial Number)",
		"CLFSH (CLFLUSH instruction)",
		"-",
		"DS (Debug Store)",
		"ACPI (Thermal Monitor and Clock Ctrl)",
		"MMX (MMX Technology)",
		"FXSR (FXSAVE/FXRSTOR)",
		"SSE",
		"SSE2",
		"SS (Self Snoop)",
		"HTT (Multi-threading)",
		"TM (Therm. Monitor)",
		"-",
		"PBE (Pend. Brk. EN.)"};
	static char* ecx_features[] = {
		"SSE3",
		"PCLMULQDQ (Carryless Multiplication)",
		"DTES64 (64-bit DS Area)",
		"MONITOR (MONITOR/MWAT)",
		"DS-CPL (CPL Qualified Debug Store)",
		"VMX (Virtual Machine Extensions)",
		"SMX (Safer Mode Extensions)",
		"EIST (Enhanced Intel SpeedStep Technology)",
		"TM2 (Thermal Monitor 2)",
		"SSSE3",
		"CNXT_ID (L1 Context ID)",
		"SDBG",
		"FMA (Fused Multiply Add)",
		"CMPXCHG16B",
		"xTPR Update Control",
		"PDCM (Perf/Debug Capability MSR)",
		"-",
		"PCID (Process-context Identifiers)",
		"DCA (Direct Cache Access)",
		"SSE4_1",
		"SSE4_2",
		"x2APIC",
		"MOVBE",
		"POPCNT",
		"TSC-Deadline",
		"AES",
		"XSAVE",
		"OSXSAVE",
		"AVX",
		"F16C",
		"RDRAND"
	};

	int _eax,_ebx,_ecx,_edx;

	ASM ("cpuid"
	     : "=a" (_eax),
	       "=b" (_ebx),
	       "=c" (_ecx),
	       "=d" (_edx)
	     : "a" (1));

	int steppingID,modelNumber,familyCode,
		processorType,extendedModel,
		extendedFamily,apicID,count,chunks,brandID;
	int model,family;

	steppingID = _eax & 0x0F; _eax >>= 4;
	modelNumber = _eax & 0x0F; _eax >>= 4;
	familyCode = _eax & 0x0F; _eax >>= 4;
	processorType = _eax & 0x03; _eax >>= 4; // two bits after those are unused
	extendedModel = _eax & 0x0F; _eax >>= 4;
	extendedFamily = _eax & 0xFF; _eax >>= 8; // rest is unused anyway.

	model = (extendedModel << 4) + modelNumber;
	family = extendedFamily + familyCode;

	brandID = _ebx & 0xFF; _ebx >>= 8;
	chunks = _ebx & 0xFF; _ebx >>= 8;
	count = _ebx & 0xFF; _ebx >>= 8;
	apicID = _ebx & 0xFF; _ebx >>= 8;

	for(int i = 0; i < sizeof(edx_features)/sizeof(char*); i++)
		if(*edx_features[i] != '-' && (_edx & (1<<i)))
				printf("%s\n", edx_features[i]);

	for(int i = 0; i < sizeof(ecx_features)/sizeof(char*); i++)
		if(*ecx_features[i] != '-' && (_ecx & (1<<i)))
				printf("%s\n", ecx_features[i]);
	printf("\n");
}
