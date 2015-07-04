#include <asm.h>
#include <printf.h>
#include <printk.h>

char* edx_features[] = {"FPU ", "VME ", "DE ", "PSE ", "TSC ", "MSR ", "PAE ",
                        "MCE ", "CX8 ", "APIC ", "-", "SEP ", "MTRR ",
                        "PGE ", "MCA ", "CMOV ", "PAT ", "PSE_36 ", "PSN ",
                        "CLFSH ", "-", "DS ", "ACPI ", "MMX ", "FXSR ",
                        "SSE ", "SSE2 ", "SS ", "HTT ", "TM ", "IA64 ", "PBE "};
char* ecx_features[] = {"SSE3 ", "-", "-", "MONITOR ", "DS_CPL ",
                        "VMX ", "-", "EST ", "TM2 ", "SSSE3 ", "CID ",
                        "-", "-", "CX16 ", "xTPR ", "-", "-",
                        "-", "DCA "};
char* edx_features_lower_case[]
= {"(fpu) ", "(vme) ", "(de) ", "(pse) ", "(tsc) ", "(msr) ", "(pae) ",
   "(mce) ", "(cx8) ", "(apic) ", "-", "(sep) ", "(mtrr) ",
   "(pge) ", "(mca) ", "(cmov) ", "(pat) ", "(pse_36) ", "(psn) ",
   "(clfsh) ", "-", "(ds) ", "(acpi) ", "(mmx) ", "(fxsr) ",
   "(sse) ", "(sse2) ", "(ss) ", "(htt) ", "(tm) ", "(ia64) ", "(pbe) "};
char* ecx_features_lower_case[]
= {"(sse3) ", "-", "-", "(monitor) ", "(ds_cpl) ",
   "(vmx) ", "-", "(est) ", "(tm2) ", "(ssse3) ", "(cid) ",
   "-", "-", "(cx16) ", "(xtpr) ", "-", "-",
   "-", "(dca) "};

void cpuid_check()
{
	int original_flags,modified_flags,read_flags,cpuid_bit;
	int cpuid_avail = 1;
	const int CPUID_BIT = 21;
	int steppingID,modelNumber,familyCode,
		processorType,extendedModel,
		extendedFamily,apicID,count,chunks,brandID;
	int model,family;
	int _eax,_ebx,_ecx,_edx;

	ASM("pushf\n"
	    "pop %0" : "=m"(original_flags));

	modified_flags = original_flags;

	cpuid_bit = original_flags & (1<<CPUID_BIT);
	if(!cpuid_bit)
		modified_flags |= (1<<CPUID_BIT);
	else
		modified_flags &= ~(1<<CPUID_BIT);

	ASM("push %0\n"
	    "popf" :: "m"(modified_flags));

	ASM("pushf\n"
	    "pop %0" : "=m"(read_flags));

	if(read_flags == original_flags || read_flags != modified_flags)
		cpuid_avail = 0;

	if(cpuid_avail)
	{
		ASM ("cpuid"
		     : "=a" (_eax),
		       "=b" (_ebx),
		       "=c" (_ecx),
		       "=d" (_edx)
		     : "a" (1));

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
			if(*edx_features[i] != '-')
			{
				printf(((_edx & (1<<i))?edx_features:edx_features_lower_case)[i]);
			}

		for(int i = 0; i < sizeof(ecx_features)/sizeof(char*); i++)
			if(*ecx_features[i] != '-')
			{
				printf(((_ecx & (1<<i))?ecx_features:ecx_features_lower_case)[i]);
			}
		printf("\n");
	}
	else
		printk(LOG "No cpuid avail\n");
}
