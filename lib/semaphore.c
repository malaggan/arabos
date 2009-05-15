#include <semaphore.h>
#include <asm.h>

void wait(volatile semaphore_t* sem)
{
    while(1)
    {
        block_timer();
        if(*sem > 0)
        {
            (*sem)--;

            //printk("\nlock obtained\n");
            unmask_timer();
            return;
        }
        else
        {
            unmask_timer();
        }
    }
    //printk("\nlock obtained ILLEGALLY\n");
}

void signal(volatile semaphore_t* sem)
{
    block_timer();
    (*sem)++;
    //printk("\nlock released\n");
    unmask_timer();
}