#include <stdio.h>
#include <getopt.h>
#include <stdlib.h>

#define PRINTF_INT(X) printf("%s = %d\n", #X, X)
#define PRINTF_STRING(X) printf("%s = %s\n", #X, X)
#define DO_STATEMENT(X) \
	X; \
	printf("%s\n", #X);

/* optind
 * The current index in argv. When the while loop 
 * has finished, remaining operands are found in 
 * argv[optind] through argv[argc-1]. (Remember that 
 * 'argv[argc] == NULL'.)

 * optopt
 * When an invalid option character is found, getopt() 
 * returns either a '?' or a ':' (see below), and optopt 
 * contains the invalid character that was found.
 */

int iflag, rflag = 0;
int clean_temps = 1;
struct option longopts[] = {
	{"install",		required_argument,	NULL,			'i'},
	{"root",		required_argument,	NULL,			'r'},
	{"keep-temp",	no_argument,		&clean_temps,	  0},
	{"help",		no_argument,		NULL,			'h'},
	{0, 0, 0, 0}
};

int main(int argc, char *argv[])
{
	int c;
	char *archive = NULL, *prefix = NULL;

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
			printf("usage message\n");
			break;
		case ':':
			fprintf(stderr, "option '%s' requires an argument\n", 
				argv[optind - 1]);
			exit(EXIT_FAILURE);
			break;
		case '?':
			fprintf(stderr, "option '%s' is invalid\n", 
				argv[optind - 1]);
			exit(EXIT_FAILURE);
			break;
		case 1:
			printf("plain argument %s\n", optarg);
			break;
		default:
			printf("default action came\n");
			break;
		}
	}

	PRINTF_STRING(prefix);
	PRINTF_STRING(archive);

	return 0;
}
