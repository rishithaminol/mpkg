#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <linux/limits.h>	/* NAME_MAX */
#include <sys/types.h>	/* mkdir() */
#include <sys/stat.h>	/* mkdir() */
#include <fcntl.h>		/* open() and its macros */

#include "main.h"	/* archive, prefix, prog_name */
#include "mpkg.h"	/* utils.h, mpkg_usage(), */
#include "copy.h"
#include "ar.h"
#include "info.h"
#include "sqlfrms.h"

char temp_str[4096];

extern int iflag, rflag, infoflag, clean_temps;

/*! @brief used by ar_grab()
 *	This function updatest temp_str string.
 */
static void _get_info(char *str, int n)
{
	static int count = 0;

	if (count > 0) {
		mpkg_err("info file size is larger than the default\n");
		exit(EXIT_FAILURE);
	}

	//info_string = strdup(str);
	strcpy(temp_str, str);
	count++;
}

/*! @brief used by copy() */
static void copy_log_(void *fd, char *fpath)
{
	char t[256];
	sprintf(t, "%s\n", fpath);

	write(*((int *)fd), t, strlen(t) + 1);
}

static int tar_extract(const char *src, const char *dest)
{
	char cmd[NAME_MAX];

	sprintf(cmd, "/bin/tar -xf %s -C %s", src, dest);

	if (system(cmd) != 0) {
		mpkg_err("error occured while extracting '%s'\n", src);
		return -1;
	}

	return 0;
}

static void prepare_tempds(void)
{
	if (file_exist(TMP_DIR) == 0) {
		printf("removing previous temp files\n");
		remove_tmpdir(TMP_DIR);
	}

	mkdir(TMP_DIR, S_IRWXU);
	mkdir(TMP_DATA_DIR, S_IRWXU);
	mkdir(TMP_CONFIG_DIR, S_IRWXU);
}

int mpkg(int argc, char *argv[])
{
	ar_object *ar1 = NULL;
	info_object *info;
	sqlite3 *db = NULL;

#ifdef DEBUG___
	PRINTF_STRING(prefix);
#endif

	/* Create environment */
	prepare_tempds();

	/* if archive open fail 'goto wind_up' */
	if ((ar1 = ar_open(archive)) == NULL)
		goto wind_up;
	ar_grab(ar1, "info", _get_info);
	info = info_load(temp_str);

	if (infoflag > 0)	/* if '--info' used */
		info_print(info, argv[infoflag]);

	if (iflag) { /* skip without installation */
		ar_extract_all(ar1, TMP_DIR);	/* extract all files to TMP_DIR */
		tar_extract(path_append(TMP_DIR, "control.tar.gz"), TMP_CONFIG_DIR);
		tar_extract(path_append(TMP_DIR, "data.tar.gz"), TMP_DATA_DIR);
	/* there should be control directory handlers */
	/* after extraction there sould be pre install function handle */

	/* file copying to the prefix */
	/* copy log */
		int fd = open(path_append(ADMINISTRATIVE_DIR, (info_get_fld(info, fld_pkg))->str),
			O_WRONLY|O_CREAT|O_EXCL, S_IRUSR|S_IWUSR);
		copy(TMP_DATA_DIR, prefix, (void *)&fd, copy_log_);
		close(fd);

	/* Database update section */
		db = open_main_db();
		update_db(db, info, path_append(ADMINISTRATIVE_DIR,
			(info_get_fld(info, fld_pkg))->str));
		close_main_db(db);
	}

	/* after copying there sould be post installation function handle */
wind_up:
	(ar1 != NULL) ? ar_close(ar1) : FALSE;
	info_unload(info);
	clean_temps ? remove_tmpdir(TMP_DIR) : FALSE;

	return 0;
}
