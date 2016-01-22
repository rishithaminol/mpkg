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

char *tmp_dir = "/tmp/mpkg";
char *tmp_data_dir = "/tmp/mpkg/data";
char *tmp_config_dir = "/tmp/mpkg/config";
char *prog_name = NULL;	/*!< @brief Program name. */
char prefix[4096];		/*!< @brief Prefix path (destination installation) */
char *archive = NULL;	/*!< @brief Archive path currently in use */

/* mpkg option flags */
int iflag, rflag = 0;
static int clean_temps = 1;	/*!< @brief temporary directory deletion flag */
struct option longopts[] = {
	{"install",		required_argument,	NULL,			'i'},
	{"root",		required_argument,	NULL,			'r'},
	{"keep-temp",	no_argument,		&clean_temps,	  0},
	{"help",		no_argument,		NULL,			'h'},
	{0, 0, 0, 0}
};

static int tar_extract(const char *src, const char *dest);
static void prepare_tempds(void);

int main(int argc, char *argv[])
{
	int ret, c;
	ar_object *ar1;

	prog_name = *argv;

	if (argc < 2)
		mpkg_usage(EXIT_FAILURE);

	opterr = 0; /* no getopt_long() error reports */

	while ((c = getopt_long(argc, argv, "i:r:h", longopts, NULL)) != -1) {
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
	ar_extract_all(ar1, tmp_dir);	/* extract all files to tmp_dir */

	tar_extract("/tmp/mpkg/control.tar.gz", tmp_config_dir);
	tar_extract("/tmp/mpkg/data.tar.gz", tmp_data_dir);

	/* there should be control directory handlers */
	/* after extraction there sould be pre install function handle */

	copy(tmp_data_dir, prefix);

	/* after copying there sould be post installation function handle */
clean_out:
	ar_close(ar1);
	clean_temps ? remove_tmpdir(tmp_dir) : FALSE;

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
	if (file_exist(tmp_dir) == 0) {
		printf("removing previous temp files\n");
		remove_tmpdir(tmp_dir);
	}

	mkdir(tmp_dir, S_IRWXU);
	mkdir(tmp_data_dir, S_IRWXU);
	mkdir(tmp_config_dir, S_IRWXU);
}

void mpkg_usage(int exit_status)
{
	printf(
		"Usage: %s [OPTIONS] [PACKAGE NAME]\n\n" \
		"Options\n" \
		" -i, --install    install given package\n" \
		" -r, --root       prefix\n" \
		"     --keep-temp  keep temp files\n" \
		" -h, --help       help message\n", prog_name);

	exit(exit_status);
}
