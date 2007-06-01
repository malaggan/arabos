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
*/

#include <printf.h>

/* 1- Pure virtual functions */
/* untested */
void __cxa_pure_virtual()
{
    // print error message
    panic("Pure virtual functions call");
}

/* 2- Global objects */
extern void (*ctors)(void);
void call_ctors()
{
    if(ctors == 0)
    {
        printf("No ctors to call\n");
        return;
    }    
    ctors();
    printf("Called ctor@0x%x\n",ctors);    
}


void *__dso_handle; /*only the address of this symbol is taken by gcc*/

struct object
{
        void (*f)(void*);
        void *p;
        void *d;
} object[32] = {0}; 
unsigned int iObject = 0;

// Why 32 ? This means that the number of global
// object I can have is only 32.
// Note: I probably should call the constructors after
// enabling new and delete, cuz if they needed them.
// TODO: when kernel exit I should call __cxa_finalize()

/* untested */
int __cxa_atexit(void (*f)(void *), void *p, void *d)
{
    printf("In __cxa_atexit() objects registered till now = %d",iObject);
        if (iObject >= 32) return -1;
        object[iObject].f = f;
        object[iObject].p = p;
        object[iObject].d = d;
        ++iObject;
        return 0;
}

/* untested */
/* This destroys all objects */
void __cxa_finalize(void *d /*unused param*/)
{
    printf("In __cxa_finalize()");
        unsigned int i = iObject;
        for (; i > 0; --i)
        {
                --iObject;
                object[iObject].f(object[iObject].p);
        }
}