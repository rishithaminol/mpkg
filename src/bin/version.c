/*! @file version.c
 *	@brief Ukussa package manager version displaying utilities.
 */
#include <stdio.h>

#include "main.h"

void mpkg_version(void)
{
	printf("%s\n", PACKAGE_VERSION);
}
