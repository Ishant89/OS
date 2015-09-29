#include<syscall.h>
#include<simics.h>
    #include<string.h>
#include<stdlib.h>
int main()
{
	void * base = (void*)0xffffc000;
	int c = new_pages(base,PAGE_SIZE);
	lprintf("Added a new page %d \n",c);
	
	int d = remove_pages(base);
	lprintf("Removed a new page %d \n",d);
	
	exit(100);
	return 0;
}
