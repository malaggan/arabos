typedef __SIZE_TYPE__ size_t;
void* operator new(size_t);
void* operator new[](size_t);
void operator delete(void *);
void operator delete[](void *);
//placement new
void* operator new(size_t, void*);
void* operator new[](size_t, void*);
void operator delete  (void*, void*);
void operator delete[](void*, void*);
