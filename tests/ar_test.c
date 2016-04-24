/* gcc -Wall -g -DDEBUG___ ../ar.c ar.c -o ar_test */
#include <stdio.h>

#include "../src/ar.h"
#include "../src/utils.h"

char *prog_name = NULL;
char *archive = NULL;

int main(int argc, char *argv[])
{
	ar_object *ar1;

	prog_name = argv[0];
	archive = argv[1];

	ar1 = ar_open(archive);
	ar_close(ar1);

	return 0;
}
