#include <stdlib.h>
#include <syscall.h>
#include <simics.h>
#include <malloc.h>
int main()
{
   // vanish();
   // exit(1);
   lprintf("Hello");
   int a = print(6,"Ishant");
   lprintf("Num chars are %d\n",a);
   char ch = getchar();
   lprintf("char is %c\n",ch);
   set_term_color(FGND_YLLW);
   set_cursor_pos(12,40);

   int x , y;
   get_cursor_pos(&x,&y);
   lprintf("a is %d and b is %d\n",x,y);
   /*char * buf = (char*)_malloc(16);
   lprintf("Addr is %p\n",buf);
   lprintf("Addr is %c\n",*buf);
   int c = readline(10,(char*)buf);
   lprintf("Readline is %d\n",c);
   */exit(50);

}
