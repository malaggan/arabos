#include <semaphore.h>
#include <asm.h>

void wait(volatile semaphore_t* sem)
{
    int old_value = 0;
    while(old_value <= 0) {
	while(*sem <= 0);
	old_value = *sem;
    }
    while(cmpxchg(sem, old_value, old_value-1));
    ASM("mfence" ::: "memory"); // http://stackoverflow.com/questions/12183311/difference-in-mfence-and-asm-volatile-memory
}

void signal(volatile semaphore_t* sem)
{
    int old = fetch_and_add(sem, +1);
    ASM("mfence" ::: "memory");
}
