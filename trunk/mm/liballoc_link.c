#include <semaphore.h>
#include <mm.h>

static semaphore_t mutex = 1;

int liballoc_lock()
{
    wait(&mutex);
    return 0;
}

int liballoc_unlock()
{
    signal(&mutex);
    return 0;
}

void* liballoc_alloc( size_t pages )
{
    // TODO only single page allowed to alloc continuously    
    if(pages > 1)
        panic("pages requested by liballoc > 1");

    return alloc_page();
}

int liballoc_free( void* ptr, size_t pages )
{
    if(pages > 1)
        panic("pages freed by liballoc > 1");

    free_page(ptr);

    return 0;
}



