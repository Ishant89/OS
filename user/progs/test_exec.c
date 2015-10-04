#include <syscall.h>
#include <simics.h>
#include <stdlib.h>
int main()
{
    char shell[] = "test";
    char * args[] = {shell, 0};

	int status = exec(shell,args);
	lprintf("Exec code is %d\n",status);
    //task_vanish(10);
    return 0;
}

