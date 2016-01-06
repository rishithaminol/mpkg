/* info file reader */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>

#include "ar.h"

char *info_extract(int fd, struct ar_hdr_ *hdr)
{
	char *buff;

	/* find 'info' file header */
	while (!(strcmp(hdr->ar_name, "info") == 0))
		hdr = hdr->next;

	lseek(fd, hdr->offset, SEEK_SET);

	buff = (char *)malloc(hdr->ar_size);
	read(fd, buff, hdr->ar_size);

	return buff;
}
