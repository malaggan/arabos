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
