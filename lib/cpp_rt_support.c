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
extern void (*ctors)(void); // function impl provided by GCC

// Note: I probably should call the constructors after
// enabling new and delete, cuz if they needed them.
void call_ctors()
{
    if(ctors == 0) // can this be true ?
    {
        printk(DEBUG "No ctors to call ( void (*ctors)(void) == 0 )\n");
        return;
    }    
    printk(TRACE "+enter call_ctors()\n");    
    ctors();
    printk(DEBUG "Called ctors (0x%x)\n",ctors);    
    printk(TRACE "-exit call_ctors()\n");    
}

// Why 32 ? This means that the number of global
// dtors I can have is only 32.
#define MAX_DTORS 32

void *__dso_handle; // only the address of this symbol is taken by GCC

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
    printk(TRACE "In __cxa_atexit() destructors registered till now = %d\n",cDtors);
    if (cDtors >= MAX_DTORS) 
        return -1;
    destructors[cDtors].func = func;
    destructors[cDtors].arg = arg;
    ++cDtors;
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

