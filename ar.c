/*! @file ar.c
 *  @brief Functions in this file is used to manage unix archive format
 *  @detail options DEBUG___
 */

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
#define AR_HDR_PRINT(X)							\
	printf("%s\n", (X)->ar_name);				\
	printf("\tdate %d\n", (int)(X)->ar_date);	\
	printf("\tuid %d\n", (int)(X)->ar_uid);		\
	printf("\tgid %d\n", (int)(X)->ar_gid);		\
	printf("\tmode %d\n", (int)(X)->ar_mode);	\
	printf("\tsize %d\n", (int)(X)->ar_size);	\
	printf("\toffset %d\n", (int)(X)->offset)
#endif

typedef struct ar_object {
	int ar_fd;
	struct ar_hdr_ *ar_hdr_table;
} ar_object;

static struct ar_hdr_ *allocate_header(void);
static struct ar_hdr_ *ar_new_header(const char *header);
static struct ar_hdr_ *ar_headers(int fd);
static int ar_test(int fd);
static int ar_free(struct ar_hdr_ *z);

static struct ar_hdr_ *allocate_header(void)
{
	return (struct ar_hdr_ *)malloc(sizeof(struct ar_hdr_));
}

/*! @brief Convert string type header
 *	@todo Header checking needs more attention.
 */
static struct ar_hdr_ *ar_new_header(const char *header)
{
	int i;
	char fname_part[16];
	char *endptr;
	struct ar_hdr_ *t = allocate_header();

	strncpy(fname_part, header, 16);
	if (strchr(fname_part, '/') == NULL) {	/* header checking */
		mpkg_err("'%s' bad ar header\n", archive);
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
	t->ar_date = strtol(header + 16, &endptr, 10);
	t->ar_uid = (unsigned int)strtol(endptr, &endptr, 10);
	t->ar_gid = (unsigned int)strtol(endptr, &endptr, 10);
	t->ar_mode = (unsigned int)strtol(endptr, &endptr, 8); /* octal */
	t->ar_size = (ssize_t)strtol(endptr, &endptr, 10);
	t->next = NULL;

	return t;
}

/*! @brief Fetch file from the given fd.
 *
 * If you provide correct fd, and the ar header info this
 * function will extract file to the destination.
 *
 * @param[in] fd File discriptor.
 * @param[in] t ar header.
 * @param[in] dest Destination Directory.
 *
 * @return return 0 on succses. returns \b EXIT_FAILURE on
 *			errors.
 */
static int ar_extract(int fd, struct ar_hdr_ *t, const char *dest)
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
		mpkg_err("'%s' %s\n", t->ar_name, strerror(_error));
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

	for (i = 0; !(i == count); i++) { /* executes 'count' times */
		n = read(fd, buff, option);
		write(fd2, buff, n);
	}

	close(fd2);

	return 0;
}

/*! @cond static int global_fd;

static void _ar_extract(char *str, int n)
{
	write(global_fd, str, n);
}

int ar_extract(ar_object *obj, const char *fname, const char *dest)
{
	struct ar_hdr_ *t;
	char d_path[256];
	int _error;

	t = ar_search(obj, fname);

	sprintf(d_path, "%s/%s", dest, t->ar_name);
	global_fd = open(d_path, O_WRONLY|O_CREAT|O_EXCL, t->ar_mode);
	_error = errno;
	if (global_fd == -1) {
		fprintf(stderr, "%s: '%s' %s\n",
			prog_name, t->ar_name, strerror(_error));
		return EXIT_FAILURE;
	}

	ar_grab(obj, fname, &_ar_extract);

	close(global_fd);

	return 0;
}
* @endcond */

void ar_extract_all(ar_object *obj, const char *dest)
{
	struct ar_hdr_ *t;

	t = obj->ar_hdr_table;

	while (!(t == NULL)) {
		ar_extract(obj->ar_fd, t, dest);
		t = t->next;
	}
}

/*! @brief return ar header table for the given fd.
 *
 * Fd should be a file discriptor to a valid ar archive.
 *
 * @param[in]	fd	Filedescriptor to the archive.
 *
 * @return	return all ar headers. On errors internal function
 *			ar_new_header() will terminate the program.
 */
static struct ar_hdr_ *ar_headers(int fd)
{
	int n; /* bytes read */
	char *header = (char *)calloc(AR_HEADER_SIZE + 1, sizeof(char));
	struct ar_hdr_ *_ar_hdr_table, **tt;
#ifdef DEBUG___
	int count = 0;
#endif

	/* set position to the first header from
	 * the begining of file "!<arch>\n"
	 */
	lseek(fd, SARMAG, SEEK_SET);

	tt = &_ar_hdr_table;

	while ((n = read(fd, header, AR_HEADER_SIZE))) {
		/* if header length < AR_HEADER_SIZE it is not a header */
		if (n < AR_HEADER_SIZE)
			break;

		*tt = ar_new_header(header);
		(*tt)->offset = lseek(fd, 0, SEEK_CUR);
		/* error occured:
		 * if first file size is odd the ar archive adds new line
		 * at the end of file. If it is even ar appends second
		 * file after the first file when creating ar archives.
		 * In this situation (*tt)->ar_size is not the actual size.
		 * This bug solves adding 1 by this statement
		 * ((*tt)->ar_size % 2)
		 */
		lseek(fd, (*tt)->ar_size + ((*tt)->ar_size % 2), SEEK_CUR);
#ifdef DEBUG___
		PRINTF_INT(count++);
		PRINTF_INT(n);
		PRINTF_STRING(header);
		AR_HDR_PRINT(*tt);
#endif
		tt = &((*tt)->next);
	}

	free(header);

	return _ar_hdr_table;
}

/*! @brief Examine ar archive */
static int ar_test(int fd)
{
	char buff[SARMAG];

	read(fd, buff, SARMAG);

	return strncmp(buff, ARMAG, SARMAG) ? -1 : 0;
}

/*! @brief free up ar_hdr_ structure or structure chain */
static int ar_free(struct ar_hdr_ *z)
{
	struct ar_hdr_ *t, *h = z;

	if (z == NULL) {
		mpkg_err("Unable to free up internal structure\n");
		return -1;
	}

	while (1) {
		t = h->next;
		free(h->ar_name);
		free(h);
		if (t == NULL)
			break;
		h = t;
	}

	return 0;
}

/*! @brief Create an ar object for the given filename
 *
 * @param [in] file Path to the archive.
 *
 * @return Return \b NULL on errors otherwise return
 * 			ar_object.
 */
ar_object *ar_open(const char *file)
{
	int fd;
	ar_object *ar_obj;

	fd = open(file, O_RDONLY);

	/* check archive format */
	if (ar_test(fd) == -1) {
		mpkg_err("'%s' bad archive\n", archive);
		close(fd);
		return NULL;
	}

	ar_obj = (ar_object *)malloc(sizeof(ar_object));

	ar_obj->ar_fd = fd;
	ar_obj->ar_hdr_table = ar_headers(ar_obj->ar_fd);

	return ar_obj;
}

int ar_close(ar_object *obj)
{
	if (obj == NULL) {
		mpkg_err("unable to close ar_object\n");
		return -1;
	}

	ar_free(obj->ar_hdr_table);
	close(obj->ar_fd);
	free(obj);

	return 0;
}

/*! @brief find the ar_hdr_ structure for the given name
 *
 * @param[in] obj ar object
 * @param[in] fname file name to search
 *
 * @return return pointer to found header if no ar_hdr_
 *			found return NULL
 */
struct ar_hdr_ *ar_search(ar_object *obj, const char *fname)
{
	struct ar_hdr_ *t = obj->ar_hdr_table;

	while (!(t == NULL) && !(strcmp(t->ar_name, fname) == 0))
		t = t->next;

	return t;
}

/*! @brief Read the file name named \b fname.
 *
 * This function needs the opened archive's file descriptor that
 * is why we use ar_object. If fname matches one of the headers
 * inside ar_object, read that file and flushes output to the
 * extern_func. extern_func may be called one or more times
 * according to the file size.
 *
 * @param [in] obj ar_object
 * @param [in] fname File name to find in the ar_object
 * @param [out] extern_func externally user defined function
 *
 * @return return 0 on sucsess. If function fails return -1.
 */
int ar_grab(ar_object *obj, const char *fname,
	void (*extern_func)(char *, int))
{
	struct ar_hdr_ *ar_hdr;
	int fd;
	int i, count, option = MAX_READ_BUFF;
	ssize_t n;
	char *buff = (char *)calloc(MAX_READ_BUFF + 1, sizeof(char));

	fd = obj->ar_fd;
	ar_hdr = ar_search(obj, fname);
	lseek(fd, ar_hdr->offset, SEEK_SET);

	if (ar_hdr == NULL)
		return -1;

	if (ar_hdr->ar_size <= MAX_READ_BUFF) {
		option = ar_hdr->ar_size;
		count = 1;
	} else if (ar_hdr->ar_size > MAX_READ_BUFF) {
		count = ar_hdr->ar_size / MAX_READ_BUFF;

		/* write mismatch bytes first */
		if (ar_hdr->ar_size % MAX_READ_BUFF) { /* if mismatch is !0 */
			n = read(fd, buff, ar_hdr->ar_size % MAX_READ_BUFF);
			extern_func(buff, n);
		}
	}

	for(i = 0; !(i == count); i++) {
		n = read(fd, buff, option);
		extern_func(buff, n);
	}

	free(buff);

	return 0;
}
