#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <linux/limits.h>	/* NAME_MAX */
#include <sys/stat.h>
#include <sys/types.h>

#include "utils.h"
#include "copy.h"
#include "ar.h"
#include "info.h"

char *tmp_dir = "/tmp/mpkg";
char *tmp_data_dir = "/tmp/mpkg/data";
char *tmp_config_dir = "/tmp/mpkg/config";
char *prefix = NULL;			/* prefix where files are installed */
char *prog_name = NULL;			/* program name */
char *archive = NULL;

static char *default_prefix = "/";

int tar_extract(const char *str, const char *dest);
void prepare_tempds(void);
void mpkg_usage(int exit_status);

int iflag, rflag = 0;
static int clean_temps = 1;
struct option longopts[] = {
	{"install",		required_argument,	NULL,			'i'},
	{"root",		required_argument,	NULL,			'r'},
	{"keep-temp",	no_argument,		&clean_temps,	  0},
	{"help",		no_argument,		NULL,			'h'},
	{0, 0, 0, 0}
};

int main(int argc, char *argv[])
{
	int ret, c, fd;
	struct ar_hdr_ *ar_hdrs, *t;

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
			prefix = optarg;
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
		prefix = default_prefix;

	/* if root is given */
	ret = file_exist(prefix);
	if ((rflag == 1) && (ret != 0)) {
		fprintf(stderr, "%s: '%s' %s\n", prog_name, prefix, strerror(ret));
		exit(EXIT_FAILURE);
	}

	PRINTF_STRING(prefix);

	/* check existance of the archive */
	if ((ret = file_exist(archive))) {
		fprintf(stderr, "%s: '%s' %s\n", prog_name, archive, strerror(ret));
		exit(EXIT_FAILURE);
	}

	/* Create environment */
	prepare_tempds();

	/* if archive open fail 'goto clean_out' */
	if (!(fd = ar_open(archive)))	goto clean_out;
	ar_hdrs = ar_headers(fd);	/* get ar headers */
	t = ar_hdrs;

	/* successfully extract all files to tmp_dir */
	while (!(t == NULL)) {
		ar_extract(fd, t, tmp_dir);
		t = t->next;
	}

	tar_extract("/tmp/mpkg/control.tar.gz", tmp_config_dir);
	tar_extract("/tmp/mpkg/data.tar.gz", tmp_data_dir);

	/* there should be control directory handlers */
	/* after extraction there sould be pre install function handle */

	copy(tmp_data_dir, prefix);

	/* after copying there sould be post installation function handle */
clean_out:
	/* free ar headers */
	ar_free(ar_hdrs);
	ar_close(fd);
	clean_temps ? remove_tmpdir(tmp_dir) : FALSE;

	return 0;
}

int tar_extract(const char *src, const char *dest)
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

void prepare_tempds(void)
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
