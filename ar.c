/* functions in this file is used to manage unix archive format */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>		/* strtol */
#include <unistd.h>
#include <errno.h>

#include "ar.h"
#include "mpkg.h"
#include "utils.h"	/* MAX_READ_BUFF */

#ifdef DEBUG___
#define NODE_PRINT(X)	\
	printf("%s\n", (X)->ar_name);	\
	printf("\tdate %d\n", (int)(X)->ar_date);	\
	printf("\tuid %d\n", (int)(X)->ar_uid);	\
	printf("\tgid %d\n", (int)(X)->ar_gid);	\
	printf("\tmode %d\n", (int)(X)->ar_mode);	\
	printf("\tsize %d\n", (int)(X)->ar_size);	\
	printf("\toffset %d\n", (int)(X)->offset)
#endif

static struct ar_hdr_ *allocate_node(void)
{
	return (struct ar_hdr_ *)malloc(sizeof(struct ar_hdr_));
}

/* convert string type header */
static struct ar_hdr_ *ar_new_node(char *header)
{
	int i;
	char fname_part[16];
	char *endptr;
	struct ar_hdr_ *t = allocate_node();

	strncpy(fname_part, header, 16);
	if (strchr(fname_part, '/') == NULL) {
		fprintf(stderr, "%s: '%s' bad ar header\n", prog_name, archive);
		/* attackers can break the program by inserting invalid
		 * headers so at this point program should break
		 */
		/*return NULL;*/
		exit(EXIT_FAILURE);
	}

	/* remove trailing slash and do null termination */
	for (i = 0; !(fname_part[i] == '/'); i++);
	fname_part[i] = '\0';

	t->ar_name = strdup(fname_part);
	/* This section convert numbers stored in a string
	 * eg:- "1450696140  1000  1000  100666  1106      "
	 * strtol() converts first number and points endptr 
	 * to the next number
	 */
	t->ar_date = (time_t)strtol(header + 16, &endptr, 10);
	t->ar_uid = (uid_t)strtol(endptr, &endptr, 10);
	t->ar_gid = (gid_t)strtol(endptr, &endptr, 10);
	t->ar_mode = (mode_t)strtol(endptr, &endptr, 8);
	t->ar_size = (off_t)strtol(endptr, &endptr, 10);
	t->next = NULL;

	return t;
}

/*
 * fetch file from the given fd if you
 * provide correct fd, and the ar header info.
 */
int ar_extract(int fd, struct ar_hdr_ *t, const char *dest)
{
	int i, n, fd2;
	int count, _error;
	int option = MAX_READ_BUFF;
	char buff[MAX_READ_BUFF];
	
	lseek(fd, t->offset, SEEK_SET);

	/* temporarily use buff */
	sprintf(buff, "%s/%s", dest, t->ar_name);
	fd2 = open(buff, O_WRONLY|O_CREAT|O_EXCL, t->ar_mode);
	_error = errno;
	if (fd2 == -1) {
		fprintf(stderr, "%s: '%s' %s\n", 
			prog_name, t->ar_name, strerror(_error));
		return EXIT_FAILURE;
	}

	if (t->ar_size <= MAX_READ_BUFF) {
		option = t->ar_size;
		count = 1;
	} else if (t->ar_size > MAX_READ_BUFF) {
		count = t->ar_size / MAX_READ_BUFF;

		/* write mismatch bytes first */
		if ((t->ar_size % MAX_READ_BUFF) > 0) {
			n = read(fd, buff, t->ar_size % MAX_READ_BUFF);
			write(fd2, buff, n);
		}
	}

	/*to be tested*/
	/*for (i = 1; !(i == count); i++) {
		n = read(fd, buff, option);
		write(fd2, buff, n);
	}*/

	for (i = 1; (n = read(fd, buff, option)); i++) {
		write(fd2, buff, n);

		if (i == count)	break;
	}

	close(fd2);

	return 0;
}

/* return ar header table */
struct ar_hdr_ *ar_headers(int fd)
{
	int n; /* bytes read */
	char header[60];
	struct ar_hdr_ *_ar_hdr_table, **tt;

	/* set position to the first header from
	 * the begining of file "!<arch>\n"
	 */
	lseek(fd, SARMAG, SEEK_SET);
	
	tt = &_ar_hdr_table;

	while ((n = read(fd, header, 60))) {
		/* if header length < 60 it is not a header */
		if (n < 60)
			break;

		*tt = ar_new_node(header);
		(*tt)->offset = lseek(fd, 0, SEEK_CUR);
		lseek(fd, (*tt)->ar_size, SEEK_CUR);
#ifdef DEBUG___
		PRINTF_INT(n);
		PRINTF_STRING(header);
		NODE_PRINT(*tt);
#endif
		tt = &((*tt)->next);
	}

	return _ar_hdr_table;
}

static int ar_test(int fd)
{
	char buff[SARMAG];

	read(fd, buff, SARMAG);

	return strncmp(buff, ARMAG, SARMAG) ? -1 : 0;
}

/* free up ar_hdr_ structure or structure chain */
int ar_free(struct ar_hdr_ *z)
{
	struct ar_hdr_ *tmp, *h = z;

	if (z == NULL) {
		fprintf(stderr, "%s: '%s' bad value\n", prog_name, __func__);
		return -1;
	}

	while (1) {
		tmp = h->next;
		free(h->ar_name);
		free(h);
		if (tmp == NULL)
			break;
		h = tmp;
	}

	return 0;
}

int ar_open(const char *file)
{
	int fd;

	fd = open(file, O_RDONLY);

	/* check archive format */
	if (ar_test(fd) == -1) {
		fprintf(stderr, "%s: '%s' bad archive\n", prog_name, archive);
		return -1;
	}

	return fd;
}

void ar_close(int fd)
{
	close(fd);
}

/*int main(int argc, char *argv[])
{
	int fd, ret;
	struct ar_hdr_ *x;

	fd = open(argv[1], O_RDONLY);

	ret = ar_test(fd);
	if (ret == -1) {
		fprintf(stderr, "%s\n", argv[1]);
		close(fd);
		exit(EXIT_FAILURE);
	}
	x = ar_headers(fd);

	ar_extract(fd, x->next->next);
	ar_free(x);
	
	close(fd);
}*/
