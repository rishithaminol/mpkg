#include <stdio.h>
#include <string.h>

#include "../ar.h"
#include "../info.h"

char *prog_name = NULL; // needed by ar.c
char *archive = NULL; // needed by ar.c

int main(int argc, char *argv[])
{
	ar_object *ar1;
	struct info_field *info;

	prog_name = argv[0];
	archive = argv[1];

	ar1 = ar_open(archive);
	info = info_load(ar1);
	info_unload(info);

	ar_close(ar1);

	return 0;
}
