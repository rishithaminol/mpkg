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

#include "mpkg.h"

#ifdef DEBUG___
void mpkg_err(char *section, const char *func, int line_num, const char *err_str)
#else
void mpkg_err(const char *err_str)
#endif
{
#ifdef DEBUG___
	fprintf(stderr, "%s:%s:%s:%d error: %s\n", prog_name,
		section, func, line_num, err_str);
#else
	fprintf(stderr, "%s: error: %s\n", prog_name, err_str);
#endif
}
