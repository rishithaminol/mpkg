#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include "main.h"	/* mpkg_usage() */
#include "mpkg.h"	/* utils.h */
#include "version.h"

/* globals for the entire program. Programmers can use
 * thes variables directly
 */
char prefix[4096];		/*!< @brief Prefix path (destination installation) */
char *prog_name = NULL;	/*!< @brief Program name. */
char *archive = NULL;	/*!< @brief Archive path currently in use */

/* flag variables */
int iflag = 0;
int rflag = 0;
int infoflag = 0;
int clean_temps = 1;	/*!< @brief temporary directory deletion flag */
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

int main(int argc, char *argv[])
{
	int c, ret;

	prog_name = *argv;

	if (argc < 2) {
		mpkg_err("too few arguments");
		mpkg_usage(EXIT_FAILURE);
	}

	opterr = 0; /* no getopt_long() error reports */

	while ((c = getopt_long(argc, argv, "p:r:hVi", longopts, NULL)) != -1) {
		switch (c) {
		case 'p':
			archive = optarg;
			/* check existance of the archive */
			if ((ret = file_exist(archive))) {
				mpkg_err("'%s' %s\n", archive, strerror(ret));
				exit(EXIT_FAILURE);
			}
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

	mpkg(argc, argv);

	return 0;
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
