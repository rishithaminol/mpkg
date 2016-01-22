/*! @file version.c
 *	@brief Ukussa package manager version displaying utilities.
 */
#include <stdio.h>

#ifndef VERSION
#define VERSION "ongoing"
#endif

const char *version = VERSION;

void ukussa_version(void)
{
	printf("%s\n", version);
}
