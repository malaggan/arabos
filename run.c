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

#ifndef BOCHSRC
# define BOCHSRC "bochsGUI"
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
//int main() { return system("bochs -f ./bochsrc"); }
int main(int argc, const char** argv) 
{ 
    char arg[10]={0};
    char cmdline[400]={0};
    strcat(cmdline,"~/Desktop/Development/FOSS/Sources/bochs/bochs/bochs -q -f ");
    if(argc > 1)
    {
        strcat(arg,argv[1]);
        char* c = arg; while(*c) {*c=toupper(*c);c++;}
        strcat(cmdline,"bochs");   
        strcat(cmdline,arg);
    }
    else
    {
        strcat(cmdline,BOCHSRC);
    }
    printf(cmdline);
    printf("\n");// flush
    int rv = system(cmdline); 
    
    printf("usage: runBochs (GUI|NOGUI)\n");
    
    return rv;
}

