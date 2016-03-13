#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "../ar.h"
#include "../info.h"
#include "../v_compare.h"

char *prog_name = NULL; // needed by ar.c
char *archive = NULL; // needed by ar.c
static char *info_string;

/*! @brief Update info_string */
static void _get_info(char *str, int n)
{
	static int count = 0;

	if (count > 0) {
		fprintf(stderr, "%s: info file size is larger than the default\n",
			prog_name);
		free(info_string);
		exit(EXIT_FAILURE);
	}

	info_string = strdup(str);
	count++;
}

/* "all,package,maintainer" */
/* options all, package, maintainer, architechture, version, dependancy, home,
 * release, description, installed-size
 */
void *info_print(info_object *info, const char *commands)
{
	struct info_field *fld_;

}

int main(int argc, char *argv[])
{
	ar_object *ar1;
	info_object *info;
	dep_object *dobj;
	struct info_field *fld_;

	prog_name = argv[0];
	archive = argv[1];

	ar1 = ar_open(archive);
	ar_grab(ar1, "info", _get_info);
	info = info_load(info_string);

	fld_ = info_get_fld(info, fld_deps);
	printf("%s\n", fld_->str);
	dobj = load_deps(fld_->str);

	unload_deps(dobj);
	info_unload(info);
	ar_close(ar1);
	free(info_string);


	return 0;
}
