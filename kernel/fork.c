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

#include <fork.h>
#include <timer.h>
#include <asm.h>
#include <lib.h>
#include <debug.h>
#include <mm.h>

void stab_data(int eip);

volatile unsigned int scheduling_started = 0;
int currentProcess=-1;
struct ProcessData processes[MAX_PROCESSES];

#define debug (0)
#define debug2 (0)

void stack_trace(int* ebp, int* esp, int* eip)
{
    int old_ebp_offset = ebp - esp;
    int* stack = esp;

    printf("stack @ (esp=0x%x,ebp=0x%x,eip=0x%x)\n",esp,ebp,eip);
    
    printf("eip ");
    stab_data(eip);

    if(old_ebp_offset == 0 || ebp == 0)
    {
        printf("end of stack trace\n");
        return;
    }

    for(int i = 0; i < 100; i ++)
    {
        printf((old_ebp_offset==i?"ebp -> ":(old_ebp_offset==i-1?"eip -> ":"")));
        if(old_ebp_offset == i - 1)
        {
            printf("esp[%d]    ",i);
            stab_data(stack[i]);
            if(stack[old_ebp_offset] == 0)
            {
                printf("end of stack trace\n");
                break;
            }
            old_ebp_offset = (int*)stack[old_ebp_offset] - esp;
        }
        else if (old_ebp_offset==i)
        {
            printf("esp[%d]=0x%x\n",i,stack[i]);
        }
        else
        {
            printf( "esp[%d]=0x%x\n",i,stack[i]);
        }
    }
}

int schedule(struct interrupt_frame *r)
{
    if(r->esp < 0x300000)
    {        
        if(currentProcess >= 0 && currentProcess < MAX_PROCESSES) // if !the first schedule ever
        {
            if debug printk("\nsched called from unknown process (esp=0x%x,currProc=%d) !\n",r->esp,currentProcess);
            if debug print_stack_trace();
            return;
        }
    }
    
    int nextProcess = (currentProcess+1)%MAX_PROCESSES;
    while(nextProcess != currentProcess)
    {
        if(processes[nextProcess].valid)
            break; // found a process
        nextProcess = (nextProcess+1)%MAX_PROCESSES;
    }

    if(currentProcess != nextProcess)
    {        
        if debug2 printk(" ( %d^%d ) ", currentProcess, nextProcess);
/*
        printk("from stack@ 0x%x, eip = ",r->esp);
        stab_data(r->eip);

        printk("to stack@ 0x%x eip = ",processes[nextProcess].frame.esp);
        stab_data(processes[nextProcess].frame.eip);
        stack_trace((int*)processes[nextProcess].frame.ebp,(int*)processes[nextProcess].frame.esp,processes[nextProcess].frame.eip);
*/
        
        // save old
        if(r->esp >= 0x260000 && currentProcess >= 0 && currentProcess < MAX_PROCESSES
                && processes[currentProcess].valid) // if !the first schedule ever :), or nor a finished process,...and also sanity check...
        {
            memcpy((unsigned char*)&processes[currentProcess].frame,(unsigned char*)r,sizeof(struct interrupt_frame)); // save this process' data
            int *stack = (int*)processes[currentProcess].frame.esp;
            if debug printk("swapping from esp = 0x%x\n", stack);
            if debug printk("stack[0] = 0x%x\n",stack[0]);
            if debug printk("stack[1] = 0x%x\n",stack[1]);
            if debug printk("stack[2] = 0x%x\n",stack[2]);
            if debug stack_trace((int*)r->ebp,(int*)r->esp,(int*)r->eip);
        }
        else
        {
            if debug printk(WARNING "not saving stack 0x%x\n",r->esp);
            if debug if(processes[currentProcess].valid)
                stack_trace((int*)r->ebp,(int*)r->esp,(int*)r->eip);
        }
        

        int *stack = (int*)processes[nextProcess].frame.esp;
        if debug printk("swapping to esp = 0x%x\n", stack);
        if debug printk("stack[0] = 0x%x\n",stack[0]);
        if debug printk("stack[1] = 0x%x\n",stack[1]);
        if debug printk("stack[2] = 0x%x\n",stack[2]);
        if debug stack_trace((int*)processes[nextProcess].frame.ebp,(int*)processes[nextProcess].frame.esp,processes[nextProcess].frame.eip);
    }
    else
	return;

    memcpy((unsigned char*)r,(unsigned char*)&processes[nextProcess].frame,sizeof(struct interrupt_frame));

    currentProcess = nextProcess;

    ((int*)processes[currentProcess].frame.esp)[2] = processes[currentProcess].frame.eip;

    ASM(
            "pushl %0\n"
            "pushl %1\n"
            "pushl %2\n"
            "outb %3,$0x20\n"
            :: 
            "m"(r->esp),
            "m"(r->ebp),
            "m"(r->eflags),
            "a"((char)0x20)
            );

    /* outb val,port */
    /* inb port, val */ // is this end of interrupt EOI? 
    ASM("outb %0,$0x20\n"::"a"((char)0x11));
    ASM("outb %0,$0x21\n"::"a"((char)0x20));
    ASM("outb %0,$0x21\n"::"a"((char)0x04));
    ASM("outb %0,$0x21\n"::"a"((char)0x01));
    ASM("outb %0,$0x21\n"::"a"((char)0x00));

    ASM(
            "popfl\n"
            "popl %%ebp\n"
            "popl %%esp\n"
            "add $8, %%esp\n" // remove the int_no and error code
            "sti\n"
            "iret\n"
            ::
            "a"(r->eax),
            "b"(r->ebx),
            "c"(r->ecx),
            "d"(r->edx),
            "S"(r->esi),
            "D"(r->edi)            
            );

    panic("schedule end line was reached !");
    return 0;
}
extern void _fini();
void taskend()
{
    //if debug
    printk(TRACE "\ntask %d ended\n", currentProcess);
    processes[currentProcess].valid = 0;
    if(currentProcess == 0)
    {
	printk("Process 0 died. Calling _fini.\n");
	_fini(); // TODO: fini does not call __cxa_finalize, although it should! 
	__cxa_finalize(/*unused*/ NULL);
    }
    while(1); // consume the rest of the time slice, till the schedule is invoked again (there's better methods)
    // do some scheduling ??
}

int spawn_handler(struct interrupt_frame *r)
{
    void (*proc)() = r->ebx; // param 1
    int procIndex = 0;
    while(procIndex < MAX_PROCESSES)
    {
        if(!processes[procIndex].valid)
            break; // found empty slot
        procIndex ++;
    }
    if(procIndex >= MAX_PROCESSES)
        return -1; // failure, no more processes

    processes[procIndex].valid = 1;
    processes[procIndex].frame = *r;
    processes[procIndex].frame.eax = 0;
    processes[procIndex].frame.ebx = 0;
    processes[procIndex].frame.ecx = 0;
    processes[procIndex].frame.edx = 0;
    processes[procIndex].frame.esi = 0;
    processes[procIndex].frame.edi = 0;

    int* stack = alloc_page();

    //stack += 1024-50; // integers, => stack += 2*1024 bytes
    stack += 1024-1;
    
    //memcpy((unsigned char *)stack,(unsigned char*)r->esp,50*sizeof(int));
    *(stack--)=taskend;
    //*(stack--)=0;
    *(stack--)=r->eflags;
    *(stack--)=r->cs;
    *(stack--)=proc;

    *(stack--)=0x221122; // err_code
    *(stack)=0x112211;


    if debug printf("\nspawn:parent stack:\n");
    if debug stack_trace((int*)r->ebp, (int*)r->esp, (int*)r->eip);

    processes[procIndex].frame.esp = stack; // TODO also alloc a new stack segment in GDT
    processes[procIndex].frame.ebp = 0;//stack+5;//+old_ebp_offset;
    processes[procIndex].frame.eax = procIndex+1; // ret val (+1, so it never gets a Zero, which is reserved for the parent process
    processes[procIndex].frame.eip = proc;

    if debug printf("\nspawn:child stack after truncation:\n");
    if debug stack_trace((int*)processes[procIndex].frame.ebp, (int*)processes[procIndex].frame.esp, (int*)processes[procIndex].frame.eip);

    r->eax = 0;
    return 0;
};

int fork_handler(struct interrupt_frame *r)
{
    int procIndex = 0;
    while(procIndex < MAX_PROCESSES)
    {
        if(!processes[procIndex].valid)
            break; // found empty slot
        procIndex ++;
    }
    if(procIndex >= MAX_PROCESSES)
        return -1; // failure, no more processes

    processes[procIndex].valid = 1;
    processes[procIndex].frame = *r;
    int* stack = alloc_page();

    stack += 1024-50; // integers, => stack += 2*1024 bytes
       
    memcpy((unsigned char *)stack,(unsigned char*)r->esp,50*sizeof(int));

    if debug printf("\nfork:parent stack:\n");
    if debug stack_trace((int*)r->ebp, (int*)r->esp, (int*)r->eip);

    //=========== update all ebp's
    int old_ebp_offset = (int*)r->ebp - (int*)r->esp;
    do{
        int next_ebp_offset =  (int*)stack[old_ebp_offset] - (int*)r->esp;
        stack[old_ebp_offset] = stack + next_ebp_offset;
        old_ebp_offset = next_ebp_offset;
    }
    while(stack[old_ebp_offset] != 0);
    //===========

    old_ebp_offset = (int*)r->ebp - (int*)r->esp;
    old_ebp_offset = (int*)stack[old_ebp_offset] - stack;
    if debug printk("older ebp found at stack[%d]\n",old_ebp_offset);
    stack[old_ebp_offset + 1] = taskend;
    stack[old_ebp_offset] = 0;

    old_ebp_offset = (int*)r->ebp - (int*)r->esp;
    processes[procIndex].frame.esp = stack; // TODO also alloc a new stack segment in GDT
    processes[procIndex].frame.ebp = stack+old_ebp_offset;
    processes[procIndex].frame.eax = procIndex+1;  // ret val (+1, so it never gets a Zero, which is reserved for the parent process

    if debug printf("\nfork:child stack:\n");
    if debug stack_trace((int*)processes[procIndex].frame.ebp, (int*)processes[procIndex].frame.esp, (int*)processes[procIndex].frame.eip);

    r->eax = 0;
    return 0;
}

int fork()
{
    ASM("int $48");
    int ret;
    reax(ret);
    return ret;
}


int spawn(void (*proc)(void))
{
    int esp; resp(esp);
    if debug printf("\nspawn stack before @ 0x%x: \n", esp);
    if debug print_stack_trace();

    ASM("movl %0,%%ebx\n"
        "int $49"::"m"(proc)); // call spawn_handler
    int ret;
    reax(ret);
    scheduling_started = 1;

    resp(esp);
    if debug printf("\nspawn stack after @ 0x%x: \n", esp);
    if debug print_stack_trace();
    
    return ret;
}

/*
     * calling convention:
     * push eip
     * push ebp
     * ebp <- esp
     * esp = esp - size
     * (..method body..)
     * esp <- ebp
     * pop ebp
     */
    /*
     * stack should look like
     * (higher address)
     * |                 |
     * | older ret ip    |
     * | older ebp       |
     * | ...             |
     * | ret ip (fork()) |
     * | old ebp         | <- ebp
     * |                 |
     */
