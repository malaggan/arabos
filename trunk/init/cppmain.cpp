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

#include <cpp.h>

extern "C" void call_ctors();
extern "C" void __cxa_finalize(void *d /*unused param*/);

void cppmain();

extern "C"
void enter_cpp()
{   
    call_ctors();
    
    printk(LOG "In C++ main\n");    
        
    cppmain();
    
    __cxa_finalize(/*unused*/ NULL);
}

class Koko { public: Koko() {printf("koko\n");} ~Koko(){printf("~koko\n");}} kk;

void cppmain()
{        
    printf(WHITE "Welcome to " REDB "ArOS" BLUEB " v 0.003_1!\n" NORMAL);	
    
    printf("Current PRINTK_LEVEL is ");
    
    if(*TRACE == *PRINTK_LEVEL)                
        printf("TRACE\n");
    else if(*DEBUG == *PRINTK_LEVEL)
        printf("DEBUG\n");
    else if(*LOG == *PRINTK_LEVEL)                
        printf("LOG\n");
    else if(*WARNING == *PRINTK_LEVEL)
        printf("WARNING\n");
    else if(*ERROR == *PRINTK_LEVEL)                
        printf("ERROR\n");
    else if(*SEVERE == *PRINTK_LEVEL)                
        printf("SEVERE\n");
        
    for(unsigned int i = 0xffffff; i; i--);
    printk(        "Testing printk\n");
    for(unsigned int i = 0xffffff; i; i--);
    printk(TRACE   "Testing printk\n");
    for(unsigned int i = 0xffffff; i; i--);
    printk(DEBUG   "Testing printk\n");
    for(unsigned int i = 0xffffff; i; i--);
    printk(LOG     "Testing printk\n");
    for(unsigned int i = 0xffffff; i; i--);
    printk(WARNING "Testing printk\n");
    for(unsigned int i = 0xffffff; i; i--);
    printk(ERROR   "Testing printk\n");
    for(unsigned int i = 0xffffff; i; i--);
    printk(SEVERE  "Testing printk\n");
    for(unsigned int i = 0xffffff; i; i--);
}


