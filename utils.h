#ifndef UTILS_H
#define UTILS_H

#include <regex.h>

#ifndef TRUE
#define TRUE (1 == 1)
#define FALSE (!TRUE)
#endif

#define MAX_READ_BUFF 65536
#define SYS_ARCH "amd64"
#define OS_CODENAME "navam"
#define MPKG_EXTENSION "mav"

#ifdef DEBUG___
	#define PRINTF_INT(X) printf("[DEBUG] %s = %d\n", #X, X)
	#define PRINTF_STRING(X) printf("[DEBUG] %s = \"%s\"\n", #X, X)
	#define DO_STATEMENT(X) \
		X; \
		printf("[DEBUG] %s\n", #X)
	#define _LINE_PASS() \
		printf("[DEBUG] %d: %s() %s PASSED\n", __LINE__, __FUNCTION__, __FILE__);
#endif

extern int file_exist(const char *filename);
extern int regcomp_e_(regex_t *preg, const char *expr);
extern int regexec_(const regex_t *preg, const char *string,
	ssize_t nmatch, regmatch_t *pmatch);

#endif /* UTILS_H */
