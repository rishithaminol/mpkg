#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <linux/limits.h>	/* NAME_MAX */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>			/* open() */
#include <sqlite3.h>
#include <unistd.h>

#include <config.h>

#include "mpkg.h"
#include "utils.h"
#include "copy.h"
#include "ar.h"
#include "info.h"
#include "sqlfrms.h"
#include "version.h"

char temp_str[4096];

/* globals for the entire program. Programmers can use
 * thes variables directly
 */
char prefix[4096];		/*!< @brief Prefix path (destination installation) */
char *prog_name = NULL;	/*!< @brief Program name. */
char *archive = NULL;	/*!< @brief Archive path currently in use */

/* flag variables */
static int iflag = 0;
static int pflag, rflag = 0;
static int infoflag = 0;
static int clean_temps = 1;	/*!< @brief temporary directory deletion flag */
struct option longopts[] = {
	{"install",     no_argument,        &iflag,         'i'},
	{"package",		required_argument,	NULL,			'p'},
	{"root",		required_argument,	NULL,			'r'},
	{"keep-temp",	no_argument,		&clean_temps,	  0},
	{"help",		no_argument,		NULL,			'h'},
	{"info",		optional_argument,	NULL,			  2},
	{"version",		no_argument,		NULL,			'V'},
	{0, 0, 0, 0}
};

static int tar_extract(const char *src, const char *dest);
static void prepare_tempds(void);

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

int main(int argc, char *argv[])
{
	int ret, c;
	ar_object *ar1;
	info_object *info;
	sqlite3 *db = NULL;

	prog_name = *argv;

	if (argc < 2) {
		mpkg_err("too few arguments");
		mpkg_usage(EXIT_FAILURE);
	}

	opterr = 0; /* no getopt_long() error reports */

	while ((c = getopt_long(argc, argv, "p:r:hVi", longopts, NULL)) != -1) {
		switch (c) {
		case 'p':
			pflag = 0;
			archive = optarg;
			break;
		case 'r':
			rflag = 1;
			strcpy(prefix, optarg);
			break;
		case 'h':
			mpkg_usage(EXIT_SUCCESS);
			break;
		case 'V':
			mpkg_version();
			exit(EXIT_SUCCESS);
			break;
		case 'i':
			iflag = 'i';
			break;
		case ':':
			mpkg_err("option '%s' requires an argument\n", argv[optind - 1]);
			exit(EXIT_FAILURE);
			break;
		case '?':
		/*default:*/
			mpkg_err("option '%s' is invalid\n", argv[optind - 1]);
			exit(EXIT_FAILURE);
			break;
		case 1:
			printf("plain argument %s\n", optarg);
			break;
		case 2:
			infoflag = optind;
			break;
		}
	}

	/* if root is given */
	if (rflag) { /* check the existance */
		ret = file_exist(prefix);
		if (ret != 0) {
			mpkg_err("'%s' %s\n", prefix, strerror(ret));
			exit(EXIT_FAILURE);
		}
	} else {
		strcpy(prefix, "/");
	}

#ifdef DEBUG___
	PRINTF_STRING(prefix);
#endif

	/* check existance of the archive */
	if ((ret = file_exist(archive))) {
		mpkg_err("'%s' %s\n", archive, strerror(ret));
		exit(EXIT_FAILURE);
	}

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
	ar_close(ar1);
	info_unload(info);
	clean_temps ? remove_tmpdir(TMP_DIR) : FALSE;

	return 0;
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

void mpkg_usage(int exit_status)
{
	printf(
		"Usage: %s [OPTIONS] [PACKAGE NAME]\n\n" \
		"Options\n" \
		" -p, --package              point package path\n" \
		" -i, --install              install given package\n" \
		" -r, --root                 prefix\n" \
		"     --keep-temp            keep temp files\n" \
		"     --info [COMMANDS, ...] print package infomation. COMMANDS are as follows.\n" \
		"                            all, package, maintainer, architecture, version,\n" \
		"                            dependancies, homepage, release, description,\n" \
		"                            installed-size\n" \
		" -h, --help                 help message\n", prog_name);

	exit(exit_status);
}
