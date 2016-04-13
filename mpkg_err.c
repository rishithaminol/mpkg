/*! @file mpgk_err.c
 *  @brief A common way of error reporting
 *  @detail This section handles all the error reporting
 *			features in all sections. Using this section
 *			the programmer can debug the code (finding the
 *			line number, section, function name). To enable
 *			debuging features programmer need to use DEBUG___
 *			macro.
 */

#include <stdio.h>
#include <string.h>

#include "mpkg.h"
#include "mpkg_err.h"

#ifdef DEBUG___
extern void mpkg_err(char *section, const char *func,
	int line_num, error_type er_type, const char *err_str)
#else
extern void mpkg_err(error_type er_type, const char *err_str)
#endif
{
	char type_str[10];

	switch (er_type) {
		case mpkg_err_error:
			strcpy(type_str, "error");
			break;

		case mpkg_err_warning:
			strcpy(type_str, "warning");
			break;
	}

#ifdef DEBUG___
	fprintf(stderr, "%s:%s:%s:%d %s: %s\n", prog_name,
		section, func, line_num, type_str, err_str);
#else
	fprintf(stderr, "%s: %s: %s\n", prog_name, type_str, err_str);
#endif
}
