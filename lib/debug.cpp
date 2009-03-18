#include <lib.h>
#include <debug.h>
#include <asm.h>

#ifdef DBG_STABS

Stab* stabs = reinterpret_cast<Stab*>(&__stab_start);
Stab* stabs_end = reinterpret_cast<Stab*>(&__stab_end);
const int stab_cnt = (reinterpret_cast<int>(&__stab_end) - reinterpret_cast<int>(&__stab_start))/sizeof(Stab);
const char* stabstr = reinterpret_cast<const char*>(&__stabstr_start);

extern "C" void print_debug_info()
{
    printf("Stab info: .stab(0x%x-0x%x). stabstr(0x%x-0x%x)\n",
        stabs,&__stab_end,stabstr,&__stabstr_end);
}

static void seek(int type, int addr, int &low, int &high)
{
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
                printf("can't find any `type=0x%x` stabs !\n",type);
                return;
            }
        }

#if 1
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

            if(addr > reinterpret_cast<int>(stabs[tmp].n_value))
            {
                low = tmp;
                continue;
            }
            else
            {
                low = oldTmp;
                break; // range founds
            }
        }
        else if(addr < reinterpret_cast<int>(stabs[tmp].n_value))
        {
            // the prev one
            int oldTmp = tmp;
            tmp--;
            while(tmp >= low && stabs[tmp].n_type != type)
                tmp--;

            if(addr < reinterpret_cast<int>(stabs[tmp].n_value))
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

            printf("ret! l %d h %d\n",low,high);

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
        printf("not found\n");
    }

    printf("ret_ l %d h %d\n",low,high);
}

static void stab_data(int eip)
{
    // seek the stabs for that address.
    printf("seeking file name 0x%x\n",eip);
    int lfile = 0, hfile= stab_cnt-1;
    seek(N_SO,eip,lfile,hfile);
    if(lfile > hfile)
    {
        printf("can't find address\n");
        return;
    }
    Stab* stab = &stabs[lfile];
    const char* fileName = stabstr+stab->n_strx;
    printf("%s lfile %d hfile %d\n",fileName, lfile, hfile);
    //==============
    printf("seeking method name 0x%x\n",eip);
    int lfun = lfile, hfun= hfile;
    seek(N_FUN,eip,lfun,hfun);
    if(lfun > hfun)
    {
        printf("can't find address\n");
        return;
    }
    stab = &stabs[lfun];
    const char* funcName = stabstr+stab->n_strx;
    printf("%s lfun %d hfun %d\n",funcName, lfun, hfun);
    //==============
    printf("seeking line number %d\n",eip-reinterpret_cast<int>(stab->n_value));
    int lline = lfun, hline = hfun;
    seek(N_SLINE,eip-reinterpret_cast<int>(stab->n_value)/*relative address*/,lline,hline);
    if(lline > hline)
    {
        printf("can't find address\n");
        return;
    }
    stab = &stabs[lline-1]; // we substract one, becaues the return address is the next line of the calling line
    int line_no = static_cast<int>(stab->n_desc);
    printf("%d lline %d hline %d\n",line_no, lline, hline);

    // search for a nested N_SOL in the N_SO, in case it was an inlined file
    while((lline > lfile && stabs[lline].n_type != N_SO && stabs[lline].n_type != N_SOL) || stabs[lline].n_value == 0)
        lline --;
    const char* inlinedFileName = NULL;
    if(lline > lfile && stabs[lline].n_strx > 0)
    {
         inlinedFileName = stabstr+stabs[lline].n_strx;
    }

    if(!inlinedFileName)
        printf("%s:%s:%d\n",fileName,funcName,line_no);
    else
        printf("%s:%s:%d\n",inlinedFileName,funcName,line_no);
}


extern "C" void print_stack_trace()
{
    unsigned int ebp;
    rebp(ebp);
    ebp+=4; // get the return address
    int retAddr = reinterpret_cast<int>(*reinterpret_cast<unsigned int **>(ebp));

    stab_data(retAddr);
}

#elif DBG_DWARF2

extern "C" void print_stack_trace()
{
    printf("no debug info available\n");
}

extern "C" void print_debug_info()
{
    printf("dwarf debug mode\n");
}
#endif /* DBG_* */