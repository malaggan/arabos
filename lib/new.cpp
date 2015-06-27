#include <liballoc.h>

void* operator new(size_t size)
{
	return malloc(size);
}

void* operator new[](size_t size)
{
	return malloc(size);
}

void operator delete(void * ptr)
{
	free(ptr);
}

void operator delete[](void * ptr)
{
	free(ptr);
}

void* operator new(size_t, void *p) { return p; }
void* operator new[](size_t, void *p) { return p; }
void operator delete  (void*, void*) {}
void operator delete[](void*, void*) {}
