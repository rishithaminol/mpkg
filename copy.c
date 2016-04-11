#include <stdlib.h>
#include <stdio.h>
#include <linux/limits.h>
#include <fts.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

/* globals */
#include "mpkg.h"

/* inode number handling table */
#include "in_list.h"

#include "copy.h"
#include "utils.h"

/* global in_list (inode list)
 * this is only for "copy.o"
 */
static struct in_list *in_list = NULL;

static int fts_compare(const FTSENT **one, const FTSENT **two);
static void copy_file(const FTSENT *f, const char *dest_path);
static void copy_link(const FTSENT *p, const char *dest_path);

static int fts_compare(const FTSENT **one, const FTSENT **two)
{
	return strcmp((*one)->fts_name, (*two)->fts_name);
}

/* copy reagular files */
static void copy_file(const FTSENT *f, const char *dest_path)
{
	int src_fd, dest_fd, option;
	int n;
	unsigned char buffer[MAX_READ_BUFF];

	option = (file_exist(dest_path) == 0)
		? O_WRONLY|O_TRUNC
		: O_WRONLY|O_CREAT|O_EXCL;

	src_fd = open(f->fts_path, O_RDONLY);
	dest_fd = open(dest_path, option, f->fts_statp->st_mode);

	while ((n = read(src_fd, buffer, MAX_READ_BUFF)))
		write(dest_fd, buffer, n);

	close(dest_fd);
	close(src_fd);
}

/*! @brief copy symlinks
 *
 *	@todo Path determinations may not work correctly.
 *			Check symlink's manual oldpath section.
 *			oldpath may lead to incorrect targets if we change
 *			prefix(destination of installation).
 */
static void copy_link(const FTSENT *f, const char *dest_path)
{
	char pointed_path[PATH_MAX];
	ssize_t n;

	n = readlink(f->fts_path, pointed_path,
		f->fts_statp->st_size + 1);
	pointed_path[n] = '\0';

	if (file_exist(dest_path) == 0)
		unlink(dest_path);

	if (symlink(pointed_path, dest_path) == -1) {
		fprintf(stderr, "%s: '%s' Warning %s\n", prog_name, __func__,
			strerror(errno));
	}
}

/* copy hard links using in_list linked struct.
 * caller already knows 'f' is a hard link.
 */
static void copy_hlink(const FTSENT *f, const char *dest)
{
	char *x = NULL;

	if (in_list != NULL)
		/* find the hlink for the given inode */
		x = in_search(f->fts_statp->st_ino, in_list);

	if (x == NULL) {	/* no previous reference found */
		if (f->fts_info == FTS_F)
			copy_file(f, dest);
		else
			copy_link(f, dest);

		if (in_list == NULL)
			/* if in_list is an empty list, in_new_segment will 
			 * create a new beginning segment for other functions to perform
			*/
			in_list = in_new_segment(f->fts_statp->st_ino, dest);
		else
			in_append(f->fts_statp->st_ino, dest, in_list);
	} else {	/* previous reference found */
		link(x, dest);
	}
}

void copy(const char *src, const char *dest,
	void *opt_, void (*callback)(void *clb_opt, char *str))
{
	int src_root_len;
	FTSENT *file = NULL;
	FTS *file_system;
	char **s = (char **)malloc(sizeof(char **) * 2);
	char a[PATH_MAX];

	*s = (char *const)src;
	*(s + 1) =  NULL;
	file_system = fts_open(s, FTS_COMFOLLOW | FTS_NOCHDIR |
		    FTS_PHYSICAL, &fts_compare);

	src_root_len = strlen(src);

	while ((file = fts_read(file_system)) != NULL) {
		/* identify the source root */
		if (file->fts_level == 0)
			continue;

		if (file->fts_errno == 0)
			sprintf(a, "%s%s",
				dest, file->fts_path + src_root_len);
		else
			printf("warning: '%s'\n", file->fts_path);

		switch (file->fts_info) {
		case FTS_D:
			/* ENOENT - no such file or directory */
			if (file_exist(a) == ENOENT)
				mkdir(a, file->fts_statp->st_mode);
			(callback != NULL) ? callback(opt_, a) : FALSE;
			break;
		case FTS_F:
		case FTS_SL:
		case FTS_SLNONE:	/* A symbolic link with a nonexistent target. */
			if (file->fts_statp->st_nlink > 1)
				copy_hlink(file, a);
			else if (file->fts_info == FTS_F)
				copy_file(file, a);
			else
				copy_link(file, a);
			(callback != NULL) ? callback(opt_, a) : FALSE;
			break;
		}
	}

	if (in_list != NULL)
		in_free(in_list);

	free(s);
	fts_close(file_system);
}

/* remove given 'f' named directory with contents */
void remove_tmpdir(const char *f)
{
	FTSENT *file = NULL;
	FTS *p = NULL;
	int tmp_errno;
	char **s = (char **)malloc(sizeof(char **) * 2);

	*s = (char * const)f;
	*(s + 1) = NULL;

	p = fts_open(s, FTS_COMFOLLOW | FTS_NOCHDIR |
		     FTS_PHYSICAL, &fts_compare);
	tmp_errno = errno;

	if (p == NULL) {
		fprintf(stderr, "%s: %s %s\n", prog_name, f, strerror(tmp_errno));
		exit(tmp_errno);
	}

	while ((file = fts_read(p)) != NULL) {
		switch (file->fts_info) {
		case FTS_DP:
		case FTS_F:
		case FTS_SL:
		case FTS_SLNONE:
			if (remove(file->fts_path) != 0) {
				fprintf(stderr, "%s: '%s' %s\n",
					prog_name, file->fts_path, strerror(errno));
			}
			break;
		}
	}

	free(s);
	fts_close(p);
}
