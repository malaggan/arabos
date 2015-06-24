#include <semaphore.h>
#include <asm.h>

void wait(volatile semaphore_t* sem)
{
    while(1)
    {
        //block_timer();
	int value = *sem;
        if(value > 0)
        {
	    if(!cmpxchg(sem, value, value-1))
            //unmask_timer();
		return;
        }
        else
        {
            //unmask_timer();
        }
    }
}

void signal(volatile semaphore_t* sem)
{
    //block_timer();
    // (*sem)++;
    cmpxchg(sem, *sem, *sem+1);    
    //unmask_timer();
}
