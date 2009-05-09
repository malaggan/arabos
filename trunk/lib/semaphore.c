#include <semaphore.h>
#include <asm.h>

void wait(volatile semaphore_t* sem)
{
    int lock = 0;
    while(!lock)
    {
        block_timer();
        if(*sem > 0)
        {
            (*sem)--;
            
            lock = 1;
        }
        unmask_timer();
    }
}

void signal(volatile semaphore_t* sem)
{
    block_timer();
    (*sem)++;
    unmask_timer();
}