/*! @file version.c
 *	@brief Ukussa package manager version displaying utilities.
 */
#include <stdio.h>

#include <config.h>

void mpkg_version(void)
{
	printf("%s\n", PACKAGE_VERSION);
}
