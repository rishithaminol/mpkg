#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <err.h>

extern int errno ;

int main ()
{
   errno = 2;
   err(2, "'%s'", "hello minol");

   printf("came to return 0\n");
   return 0;
}
