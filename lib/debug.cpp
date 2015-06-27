#include <lib.h>
#include <debug.h>
#include <asm.h>

#ifdef DBG_STABS

Stab* stabs = reinterpret_cast<Stab*>(&__stab_start);
Stab* stabs_end = reinterpret_cast<Stab*>(&__stab_end);
char* stabstr = reinterpret_cast<char*>(&__stabstr_start);

void print_debug_info()
{
	printf("Stab info: .stab(0x%x-0x%x) (%d stabs). stabstr(0x%x-0x%x)\n",
	       stabs,&__stab_end, stabs_end-stabs,stabstr,&__stabstr_end);
}
#define D 0
static void seek(int type, int addr, int &low, int &high)
{
#if D
	printf("seek(type = 0x%x, addr = 0x%x, low = %d, high = %d)\n",type,addr,low,high);
#endif
	const int orig_low = low, orig_high = high;
	while(high > low)
	{
		int m = (high + low) / 2;

		// find the first middle `type` stab.
		int tmp = m;
		while(tmp >= low && stabs[tmp].n_type != type)
			tmp--;

		if(tmp < low) // no `type` stab in range [l,m]
		{
			// find the next `type` stab
			tmp = m;
			while(tmp <= high && stabs[tmp].n_type != type)
				tmp++;

			if(tmp > high)
			{
#if D
				printf("can't find any `type=0x%x` stabs !\n",type);
#endif
				return;
			}
		}

#if D
		printf("l %d h %d m %d\n",low,high,tmp);
		printf("%d type 0x%x nvalue 0x%x ndesc 0x%x nstrx 0x%x\n",
		       tmp, stabs[tmp].n_type, stabs[tmp].n_value, stabs[tmp].n_desc, stabs[tmp].n_strx);
#endif

		if(addr > reinterpret_cast<int>(stabs[tmp].n_value))
		{
			// the next one
			int oldTmp = tmp;
			tmp++;
			while(tmp <= high && stabs[tmp].n_type != type)
				tmp++;

			if(tmp <= high && addr >= reinterpret_cast<int>(stabs[tmp].n_value))
			{
				low = tmp;
				continue;
			}
			else
			{
				low = oldTmp;
				break; // range found
			}
		}
		else if(addr < reinterpret_cast<int>(stabs[tmp].n_value))
		{
			// the prev one
			int oldTmp = tmp;
			tmp--;
			while(tmp >= low && stabs[tmp].n_type != type)
				tmp--;

			if(tmp >= low && addr <= reinterpret_cast<int>(stabs[tmp].n_value))
			{
				high = tmp;
				continue;
			}
			else
			{
				high = oldTmp;
				break; // range found
			}
		}
		else // exact match
		{
			low = tmp;

			// get the next one
			tmp++;
			while(tmp <= high && stabs[tmp].n_type != type)
				tmp++;
#if D
			printf("ret! l %d h %d\n",low,high);
#endif
			return;
		}
	}

	if(stabs[low].n_type == type && addr > reinterpret_cast<int>(stabs[low].n_value))
	{
		int tmp = low+1;
		// get the next one
		while(tmp <= orig_high && stabs[tmp].n_type != type)
			tmp++;
		high = tmp;
	}
	else if(stabs[high].n_type == type && addr < reinterpret_cast<int>(stabs[high].n_value))
	{
		int tmp = high-1;
		// the prev one
		while(tmp >= orig_low && stabs[tmp].n_type != type)
			tmp--;
		low = tmp;
	}
	else
	{
		// not found
#if D
		printf("not found\n");
#endif
	}
#if D
	printf("ret_ l %d h %d\n",low,high);
#endif
}

void stab_data(int eip)
{
	int debug = D;
	int stab_cnt = stabs_end-stabs;
	// seek the stabs for that address.
	if(debug) printf("seeking file name 0x%x\n",eip);
	int lfile = 0, hfile= stab_cnt-1;
	seek(N_SO,eip,lfile,hfile);
	if(lfile > hfile)
	{
		printf("0x%x can't find file\n",eip);
		return;
	}
	Stab* stab = &stabs[lfile];
	const char* fileName = stabstr+stab->n_strx;
	if(debug) printf("%s lfile %d hfile %d\n",fileName, lfile, hfile);
	//==============
	if(debug) printf("seeking method name 0x%x\n",eip);
	int lfun = lfile, hfun= hfile;
	seek(N_FUN,eip,lfun,hfun);
	if(lfun > hfun)
	{
		printf("0x%x can't find method\n",eip);
		return;
	}
	stab = &stabs[lfun];
	const char* funcName = stabstr+stab->n_strx;
	if(debug) printf("%s lfun %d hfun %d\n",funcName, lfun, hfun);
	//==============
	if(debug) printf("seeking line number %d\n",eip-reinterpret_cast<int>(stab->n_value));
	int lline = lfun, hline = hfun;
	seek(N_SLINE,eip-reinterpret_cast<int>(stab->n_value)/*relative address*/,lline,hline);
	if(lline > hline)
	{
		// try again with non-relative addresses this time
		lline = lfun;
		hline = hfun;
		seek(N_SLINE,eip,lline,hline);
		if(lline > hline)
		{
			printf("can't find line\n");
			return;
		}
	}
	stab = &stabs[lline-1]; // we substract one, becaues the return address is the next line of the calling line
	int line_no = static_cast<int>(stab->n_desc);
	if(debug) printf("%d lline %d hline %d\n",line_no, lline, hline);

	// search for a nested N_SOL in the N_SO, in case it was an inlined file
	lline+=2; // this line is a hack, dunno why it works or if it will work everytime
	while((lline > lfile && stabs[lline].n_type != N_SO && stabs[lline].n_type != N_SOL) || stabs[lline].n_value == 0)
		lline --;
	const char* inlinedFileName = nullptr;
	if(lline > lfile && stabs[lline].n_strx > 0)
	{
		inlinedFileName = stabstr+stabs[lline].n_strx;
	}

	if(!inlinedFileName)
		printf("0x%x %s:%s:%d\n",eip, fileName,funcName,line_no);
	else
		printf("0x%x %s:%s:%d\n",eip, inlinedFileName,funcName,line_no);
}


void print_stack_trace()
{
	int* ebp;
	rebp(ebp);

	while(ebp != 0)
	{
		int prev_ebp = ebp[0];
		//ebp+=4; // get the return address
		int retAddr = ebp[1];

		stab_data(retAddr);
		ebp = reinterpret_cast<int*>(prev_ebp);
	}
}

#elif DBG_DWARF2

void print_stack_trace()
{
	printf("no debug info available\n");
}

void print_debug_info()
{
	printf("dwarf debug mode\n");
}
#endif /* DBG_* */
