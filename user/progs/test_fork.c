#include <syscall.h>
#include <simics.h>
#include <stdlib.h>
int main()
{
	int pid;
	int status_ptr ;
    char shell[] = "test";
	char * args[] = {shell, 0};
	pid = fork();

	if(!pid)
	{
		lprintf("Hello from Child\n");
		int status = exec(shell,args);
		lprintf("Exec code is %d\n",status);
	}
	else
	{
		wait(&status_ptr);
		lprintf("Hello from parent: child exited with exit status:%d\n",status_ptr);
	}
	//task_vanish(10);
	return 0;
}
