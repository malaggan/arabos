/* This file is to add runtime support functions for C++
 * To support:
 * 1- Pure virtual functions (done, not yet tested)
 * 2- Global objects
 * 2- RTTI
 * 4- Exceptions
 * Note: Templates does not need runtime support
 * Note: Done with help from http://www.osdev.org/osfaq2/index.php/Doing%20a%20kernel%20in%20C%2B%2B
 * > When kernel exit I should call __cxa_finalize(NULL) to call all the global dtors
 */

#include <lib.h>

/* 1- Pure virtual functions */
/* untested */
void __cxa_pure_virtual()
{
	// print error message
	panic("Pure virtual functions call");
}

/* 2- Global objects */

// Why 32 ? This means that the number of global
// dtors I can have is only 32.
#define MAX_DTORS 32

// void *__dso_handle; // only the address of this symbol is taken by GCC

struct dtor
{
	void (*func)(void*);
	void *arg;
} destructors[MAX_DTORS] = {{0,0}};
unsigned int cDtors = 0;

/*
 * __cxa_atexit -- register a function to be called by exit() or atexit() or when a shared library is unloaded
 * http://refspecs.freestandards.org/LSB_3.1.0/LSB-Core-generic/LSB-Core-generic/baselib---cxa-atexit.html
 */
int __cxa_atexit(void (*func)(void *), void *arg, void /*unused*/ *dso_handle)
{
	if (cDtors >= MAX_DTORS)
		return -1;
	destructors[cDtors].func = func;
	destructors[cDtors].arg = arg;
	++cDtors;
	printk(TRACE "In __cxa_atexit(%x) destructors registered till now = %d\n",func,cDtors);
	return 0;
}

/* When __cxa_finalize(d) is called, it should walk the termination function
 * list, calling each in turn if d matches dso_handle for the termination
 * function entry. If d == NULL, it should call all of them. Multiple calls to
 * __cxa_finalize shall not result in calling termination function entries
 * multiple times; the implementation may either remove entries or mark them
 * finished.
 * http://www.codesourcery.com/cxx-abi/abi.html
 * NOTE: we don't use dynamic library loading and unloading, so we can safely
 * ignore dso_handle
 */
void __cxa_finalize(void /* unsed */ *dso_handle)
{
	printk(TRACE "+enter __cxa_finalize()\n");
	while(cDtors)
	{
		cDtors --;
		destructors[cDtors].func(destructors[cDtors].arg);
	}
	printk(TRACE "-exit __cxa_finalize()\n");
}
