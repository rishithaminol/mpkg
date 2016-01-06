/* functions in this file is used to manage unix archive format */

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>		/* strtol */
#include <unistd.h>
#include <errno.h>
#include <ar.h>

#define READ_BUFF 65536

struct ar_hdr_ {
	char 	*ar_name;	/* Member file name. / terminated */
	time_t 	ar_date;	/* File date unix Epoch format */
	uid_t	ar_uid;		/* User id */
	gid_t	ar_gid;		/* Group id */
	mode_t 	ar_mode;	/* File mode in octal */
	off_t 	ar_size;	/* File size */
	off_t	offset;		/* Offset in the main archive */
	struct ar_hdr_ *next;	/* next header */
};

#ifdef DEBUG___
void node_print(struct ar_hdr_ *x)
{
	printf("%s\n", x->ar_name);
	printf("\tdate %d\n", (int)x->ar_date);
	printf("\tuid %d\n", (int)x->ar_uid);
	printf("\tgid %d\n", (int)x->ar_gid);
	printf("\tmode %d\n", (int)x->ar_mode);
	printf("\tsize %d\n", (int)x->ar_size);
	printf("\toffset %d\n", (int)x->offset);
}
#endif

static struct ar_hdr_ *allocate_node(void)
{
	return (struct ar_hdr_ *)malloc(sizeof(struct ar_hdr_));
}

/* all the conversions suitable for _ar_hdr are done here 
 */
static struct ar_hdr_ *ar_new_node(char *str)
{
	int i;
	char s[16];
	char *endptr;
	struct ar_hdr_ *t = allocate_node();

	strncpy(s, str, 16);
	if (strchr(s, '/') == NULL) {
		fprintf(stderr, "error: bad header\n");
		return NULL;
	}

	/* remove trailing slash and do null termination */
	for (i = 0; !(s[i] == '/'); i++);
	s[i] = '\0';

	t->ar_name = strdup(s);
	/* This section convert numbers stored in a string
	 * eg:- "1450696140  1000  1000  100666  1106      "
	 * strtol() converts first number and points endptr 
	 * to the next number
	 */
	t->ar_date = (time_t)strtol(str + 16, &endptr, 10);
	t->ar_uid = (uid_t)strtol(endptr, &endptr, 10);
	t->ar_gid = (gid_t)strtol(endptr, &endptr, 10);
	t->ar_mode = (mode_t)strtol(endptr, &endptr, 8);
	t->ar_size = (off_t)strtol(endptr, &endptr, 10);
	t->next = NULL;

	return t;
}

int ar_test(int fd)
{
	char buff[SARMAG];

	read(fd, buff, SARMAG);

	return strncmp(buff, ARMAG, SARMAG) ? -1 : 0;
}

static int ar_extract(int fd, struct ar_hdr_ *t)
{
	int i, n, fd2;
	int count, _error;
	int option = READ_BUFF;
	char buff[READ_BUFF];
	
	lseek(fd, t->offset, SEEK_SET);

	fd2 = open(t->ar_name, O_WRONLY|O_CREAT|O_EXCL, t->ar_mode);
	_error = errno;
	if (fd2 == -1) {
		fprintf(stderr, "error: '%s' %s\n", t->ar_name, strerror(_error));
		return EXIT_FAILURE;
	}

	if (t->ar_size <= READ_BUFF) {
		option = t->ar_size;
		count = 1;
	} else if (t->ar_size > READ_BUFF) {
		count = t->ar_size / READ_BUFF;

		/* write mismatch bytes first */
		if ((t->ar_size % READ_BUFF) > 0) {
			n = read(fd, buff, t->ar_size % READ_BUFF);
			write(fd2, buff, n);
		}
	}

	// to be tested
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

struct ar_hdr_ *ar_headers(int fd)
{
	int n; /* bytes read */
	char header[60];
	struct ar_hdr_ *_ar_hdr_table, *t;

	/* set position to the first header from
	 * the begining of file
	 */
	lseek(fd, SARMAG, SEEK_SET);
	
	/* first time initial */
	n = read(fd, header, 60);
	_ar_hdr_table = ar_new_node(header);
	_ar_hdr_table->offset = lseek(fd, 0, SEEK_CUR);	/* get current position */
	lseek(fd, _ar_hdr_table->ar_size, SEEK_CUR);

	t = _ar_hdr_table;
#ifdef DEBUG___
	node_print(t);
#endif

	while ((n = read(fd, header, 60))) {
		/* if header length < 60 it is not a header */
		if (n < 60)
			break;

		t->next = ar_new_node(header);
		t->next->offset = lseek(fd, 0, SEEK_CUR);
		lseek(fd, t->next->ar_size, SEEK_CUR);
		t = t->next;
#ifdef DEBUG___
		node_print(t);
#endif
	}

	return _ar_hdr_table;
}

void ar_free(struct ar_hdr_ *z)
{
	struct ar_hdr_ *tmp, *h = z;

	while (1) {
		tmp = h->next;
		free(h->ar_name);
		free(h);
		if (tmp == NULL)
			break;
		h = tmp;
	}
}

int main(int argc, char *argv[])
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
}
