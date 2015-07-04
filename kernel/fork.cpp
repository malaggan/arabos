#include <panic.h>
#include <types.h>
#include <fork.h>
#include <timer.h>
#include <asm.h>
#include <memops.h>
#include <debug.h>
#include <printf.h>
#include <printk.h>

void* alloc_page(); // TODO: put in a header file

#ifdef DBG_STABS
void stab_data(int eip);
#endif

volatile unsigned int scheduling_started = 0;
int currentProcess=-1;
struct ProcessData processes[MAX_PROCESSES];

#define debug (0)
#define debug2 (0)

void stack_trace(int* ebp, int* esp, int* eip)
{
	int old_ebp_offset = ebp - esp;
	int* stack = esp;

	printf("stack @ (esp=%p,ebp=%p,eip=%p)\n",esp,ebp,eip);

	printf("eip ");
#ifdef DBG_STABS
	stab_data(eip);
#endif

	if(old_ebp_offset == 0 || ebp == 0)
	{
		printf("end of stack trace\n");
		return;
	}

	for(int i = 0; i < 500; i ++)
	{
		printf((old_ebp_offset==i?"ebp -> ":(old_ebp_offset==i-1?"eip -> ":"")));
		if(old_ebp_offset == i - 1)
		{
			printf("esp[%d]    ",i);
#ifdef DBG_STABS
			stab_data(stack[i]);
#endif
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
			if(stack[i])
				printf("esp[%d]=0x%x\n",i,stack[i]);
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
			return -1;
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
		if debug stack_trace(reinterpret_cast<int*>(processes[nextProcess].frame.ebp),reinterpret_cast<int*>(processes[nextProcess].frame.esp),reinterpret_cast<int*>(processes[nextProcess].frame.eip));
	}
	else
		return -1; // TODO: what is this case?

	memcpy((unsigned char*)r,(unsigned char*)&processes[nextProcess].frame,sizeof(struct interrupt_frame));

	currentProcess = nextProcess;

	(reinterpret_cast<int*>(processes[currentProcess].frame.esp))[2] = processes[currentProcess].frame.eip;

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
	// end of interrupt EOI? (return to new process)
	ASM("outb %0,$0x20\n"::"a"((char)0x11));
	ASM("outb %0,$0x21\n"::"a"((char)0x20));
	ASM("outb %0,$0x21\n"::"a"((char)0x04));
	ASM("outb %0,$0x21\n"::"a"((char)0x01));
	ASM("outb %0,$0x21\n"::"a"((char)0x00));

	ASM(
		"popfl\n" // pop flags register
		"popl %%ebp\n"
		"popl %%esp\n"
		"add $8, %%esp\n" // remove the int_no and error code
		"sti\n"
		"iret\n" // end interrupt (once)
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
extern "C" void _fini();// extern "C" because generated by compiler and we cannot control its mangling
extern "C" void __cxa_finalize(void /* unsed */ *dso_handle);
void taskend()
{
	//if debug
	printk(TRACE "\ntask %d ended\n", currentProcess);
	processes[currentProcess].valid = 0;
	if(currentProcess == 0)
	{
		printk("Process 0 died. Calling _fini.\n");
		_fini(); // TODO: fini does not call __cxa_finalize, although it should!
		__cxa_finalize(/*unused*/ nullptr);
	}
	while(1); // consume the rest of the time slice, till the schedule is invoked again (there's better methods)
	// do some scheduling ??
}

int spawn_handler(struct interrupt_frame *r)
{
	unsigned int proc /*function pointer*/ = r->ebx; // param 1
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

	int* stack = reinterpret_cast<int*>(alloc_page());

	//stack += 1024-50; // integers, => stack += 2*1024 bytes
	stack += 1024-1 /*end of page*/; // end of page = beginning of stack. (stack grows updwards)

	//memcpy((unsigned char *)stack,(unsigned char*)r->esp,50*sizeof(int));
	*(stack--)=reinterpret_cast<int>(reinterpret_cast<int*>(taskend));
	*(stack--)=0; // new process %ebp is zero, so there is no previous stack frame and so no need to put on the stack (another interpretation is that this is an interrupt frame, and in interrupt frames, there is no saved ebp. check the irq code to know what to put on the stack (cf. popa instruction?)
	*(stack--)=r->eflags; // this is not a normal stack frame; it is an interrupt frame.. that of `schedule'
	*(stack--)=r->cs;
	*(stack--)=proc; // .. and to here the interrupt would return
	*(stack--)=0x221122; // err_code
	*(stack)=0x112211;


	if debug printf("\nspawn:parent stack:\n");
	if debug stack_trace(reinterpret_cast<int*>(r->ebp), reinterpret_cast<int*>(r->esp), reinterpret_cast<int*>(r->eip));

	processes[procIndex].frame.esp = reinterpret_cast<unsigned int>(stack); // TODO also alloc a new stack segment in GDT
	processes[procIndex].frame.ebp = 0;//stack+5;//+old_ebp_offset;
	processes[procIndex].frame.eax = procIndex+1; // ret val (+1, so it never gets a Zero, which is reserved for the parent process
	processes[procIndex].frame.eip = proc; // this is a normal stack frame, since `proc' isn't going to execute iret!

	if debug printf("\nspawn:child stack after truncation:\n");
	if debug stack_trace(reinterpret_cast<int*>(processes[procIndex].frame.ebp), reinterpret_cast<int*>(processes[procIndex].frame.esp), reinterpret_cast<int*>(processes[procIndex].frame.eip));

	r->eax = 0;
	return 0;
}

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
	int* stack = reinterpret_cast<int*>(alloc_page());

	stack += 1024-500; // integers, => stack += 2*1024 bytes // FIXME: why 500? should we copy the entire stack ? (no problem since esp is not affected anyway)

	memcpy((unsigned char *)stack,(unsigned char*)r->esp,500*sizeof(int));

	if debug printf("\nfork:parent stack:\n");
	if debug stack_trace(reinterpret_cast<int*>(r->ebp), reinterpret_cast<int*>(r->esp), reinterpret_cast<int*>(r->eip));
	if debug printf("\nfork: END parent stack\n");

	//=========== update all ebp's
	int old_ebp_offset = reinterpret_cast<int*>(r->ebp) - reinterpret_cast<int*>(r->esp);
	do{
		if debug printf("old_ebp_offset = %d\n", old_ebp_offset);
		int next_ebp_offset =  reinterpret_cast<int*>(stack[old_ebp_offset]) - reinterpret_cast<int*>(r->esp);
		if debug printf("next_ebp_offset = %d\n", next_ebp_offset);
		stack[old_ebp_offset] = reinterpret_cast<int>(stack + next_ebp_offset);
		if debug printf("A\n");
		old_ebp_offset = next_ebp_offset;
	}
	while(stack[old_ebp_offset] != 0);
	//===========

	old_ebp_offset = reinterpret_cast<int*>(r->ebp) - reinterpret_cast<int*>(r->esp);
	old_ebp_offset = reinterpret_cast<int*>(stack[old_ebp_offset]) - stack;
	if debug printk("older ebp found at stack[%d]\n",old_ebp_offset);
	stack[old_ebp_offset + 1] = reinterpret_cast<int>(reinterpret_cast<int*>(taskend));
	stack[old_ebp_offset] = 0;

	old_ebp_offset = reinterpret_cast<int*>(r->ebp) - reinterpret_cast<int*>(r->esp);
	processes[procIndex].frame.esp = reinterpret_cast<unsigned int>(stack); // TODO also alloc a new stack segment in GDT
	processes[procIndex].frame.ebp = reinterpret_cast<unsigned int>(stack+old_ebp_offset);
	processes[procIndex].frame.eax = procIndex+1;  // ret val (+1, so it never gets a Zero, which is reserved for the parent process

	if debug printf("\nfork:child stack:\n");
	if debug stack_trace(reinterpret_cast<int*>(processes[procIndex].frame.ebp), reinterpret_cast<int*>(processes[procIndex].frame.esp), reinterpret_cast<int*>(processes[procIndex].frame.eip));

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
