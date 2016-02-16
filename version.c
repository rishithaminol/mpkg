/*! @file version.c
 *	@brief Ukussa package manager version displaying utilities.
 */
#include <stdio.h>

#ifndef VERSION
#define VERSION "ongoing"
#endif

void mpkg_version(void)
{
	printf("%s\n", VERSION);
}
