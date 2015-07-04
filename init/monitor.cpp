#include <printf.h>
#include <string.h>
#include <debug.h>
void readline(char* buf, int max);
char* alloc_page();

void print_monitor_help()
{
	printf("This is the ArOS monitor shell.\n"
	       "Commands:\n"
	       "alloc_page - allocates a new page\n"
	       "h - shows this screen\n"
	       "quit - quits the monitor shell\n"
	       "pst - print stack trace (of the monitor itself)");

}

void monitor()
{
	char cmd[100];
	printf("The MONITOR ! - type h for help");

	for(;;)
	{
		printf("\n%% ");
		*cmd=0;
		readline(cmd,99);


		// handle the command
		if(!strncmp(cmd,"alloc_page",10))
			printf("0x%x\n",alloc_page());
		else if(!strncmp(cmd,"h",1))
			print_monitor_help();
		else if(!strncmp(cmd,"pst",3))
			print_stack_trace();
		else if(!strncmp(cmd,"quit",4))
			return;
		else
			printf("unrecognized montior command\n");
	}
}
