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
#include <stdarg.h>

#include "mpkg.h"
#include "mpkg_err.h"

#ifdef DEBUG___
void mpkg_err1(char *section, const char *func,	int line_num,
	const char *err_str, ...)
#else
void mpkg_err1(const char *err_str, ...)
#endif
{
	va_list args;
	char debug_string[256] = "";
	char temp_string[4096];

#ifdef DEBUG___
	sprintf(debug_string, "%s:%s:%d", section, func, line_num);
#endif

	va_start(args, err_str);
	vsprintf(temp_string, err_str, args);
	va_end(args);

	fprintf(stderr, "%s:%s error: %s\n", prog_name, debug_string, temp_string);
}

#ifdef DEBUG___
void mpkg_err2(char *section, const char *func,	int line_num,
	const char *warn_str, ...)
#else
void mpkg_err2(const char *warn_str, ...)
#endif
{
	va_list args;
	char debug_string[256] = "";
	char temp_string[4096];

#ifdef DEBUG___
	sprintf(debug_string, "%s:%s:%d", section, func, line_num);
#endif

	va_start(args, warn_str);
	vsprintf(temp_string, warn_str, args);
	va_end(args);

	fprintf(stderr, "%s:%s warning: %s\n", prog_name, debug_string, temp_string);
}
