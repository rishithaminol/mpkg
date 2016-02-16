#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <linux/limits.h>	/* NAME_MAX */
#include <sys/types.h>
#include <sys/stat.h>

#include "mpkg.h"
#include "utils.h"
#include "copy.h"
#include "ar.h"
#include "info.h"
#include "version.h"

char temp_str[4096];	/* keep temporary strings */

char prefix[4096];		/*!< @brief Prefix path (destination installation) */
char *prog_name = NULL;	/*!< @brief Program name. */
char *archive = NULL;	/*!< @brief Archive path currently in use */

/* flag variables */
int iflag, rflag = 0;
static int infoflag = 0;
static int clean_temps = 1;	/*!< @brief temporary directory deletion flag */
struct option longopts[] = {
	{"install",		required_argument,	NULL,			'i'},
	{"root",		required_argument,	NULL,			'r'},
	{"keep-temp",	no_argument,		&clean_temps,	  0},
	{"help",		no_argument,		NULL,			'h'},
	{"info",		optional_argument,	NULL,			  2},
	{"version",		no_argument,		NULL,			'V'},
	{0, 0, 0, 0}
};

static int tar_extract(const char *src, const char *dest);
static void prepare_tempds(void);

/*! @brief Update temp_str */
static void _get_info(char *str, int n)
{
	static int count = 0;

	if (count > 0) {
		fprintf(stderr, "%s: info file size is larger than the default\n",
			prog_name);
		exit(EXIT_FAILURE);
	}

	//info_string = strdup(str);
	strcpy(temp_str, str);
	count++;
}

int main(int argc, char *argv[])
{
	int ret, c;
	ar_object *ar1;

	prog_name = *argv;

	if (argc < 2)
		mpkg_usage(EXIT_FAILURE);

	opterr = 0; /* no getopt_long() error reports */

	while ((c = getopt_long(argc, argv, "i:r:hV", longopts, NULL)) != -1) {
		switch (c) {
		case 'i':
			iflag = 1;
			archive = optarg;
			break;
		case 'r':
			rflag = 1;
			/*prefix = optarg;*/
			strcpy(prefix, optarg);
			break;
		case 'h':
			mpkg_usage(EXIT_SUCCESS);
			break;
		case 'V':
			mpkg_version();
			exit(EXIT_SUCCESS);
			break;
		case ':':
			fprintf(stderr, "%s: option '%s' requires an argument\n", 
				prog_name, argv[optind - 1]);
			exit(EXIT_FAILURE);
			break;
		case '?':
		/*default:*/
			fprintf(stderr, "%s: option '%s' is invalid\n", 
				prog_name, argv[optind - 1]);
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

	/* if root is not given */
	if (rflag == 0)
		strcpy(prefix, "/");

	/* if root is given */
	ret = file_exist(prefix);
	if ((rflag == 1) && (ret != 0)) {
		fprintf(stderr, "%s: '%s' %s\n", prog_name, prefix, strerror(ret));
		exit(EXIT_FAILURE);
	}

#ifdef DEBUG___
	PRINTF_STRING(prefix);
#endif

	/* check existance of the archive */
	if ((ret = file_exist(archive))) {
		fprintf(stderr, "%s: '%s' %s\n", prog_name, archive, strerror(ret));
		exit(EXIT_FAILURE);
	}

	/* Create environment */
	prepare_tempds();

	/* if archive open fail 'goto clean_out' */
	ar1 = ar_open(archive);
	if (ar1 == NULL)	goto clean_out;

	if (infoflag > 0) {	/* if '--info' used */
		info_object *info;

		printf("info details\n");
		ar_grab(ar1, "info", _get_info);
		info = info_load(temp_str);
		info_print(info, argv[infoflag]);
		info_unload(info);
		goto clean_out;
	}

	ar_extract_all(ar1, TMP_DIR);	/* extract all files to TMP_DIR */
	tar_extract("/tmp/mpkg/control.tar.gz", TMP_CONFIG_DIR);
	tar_extract("/tmp/mpkg/data.tar.gz", TMP_DATA_DIR);

	/* there should be control directory handlers */
	/* after extraction there sould be pre install function handle */

	copy(TMP_DATA_DIR, prefix);

	/* after copying there sould be post installation function handle */
clean_out:
	ar_close(ar1);
	clean_temps ? remove_tmpdir(TMP_DIR) : FALSE;

	return 0;
}

static int tar_extract(const char *src, const char *dest)
{
	char cmd[NAME_MAX];

	sprintf(cmd, "/bin/tar -xf %s", src);
	sprintf(cmd, "%s -C %s", cmd, dest);

	if (system(cmd) != 0) {
		printf("%s: error occured while extracting '%s'\n", prog_name, src);
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
		" -i, --install    install given package\n" \
		" -r, --root       prefix\n" \
		"     --keep-temp  keep temp files\n" \
		"     --info       print package infomation.\n" \
		" -h, --help       help message\n", prog_name);

	exit(exit_status);
}
