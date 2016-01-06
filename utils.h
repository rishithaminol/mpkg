#ifndef TRUE
#define TRUE (1 == 1)
#define FALSE (!TRUE)
#endif

#define MAX_READ_BUFF 65536

#ifdef DEBUG___
	#define PRINTF_INT(X) printf("%s = %d\n", #X, X)
	#define PRINTF_STRING(X) printf("%s = \"%s\"\n", #X, X)
	#define DO_STATEMENT(X) \
		X; \
		printf("%s\n", #X)
	#define _LINE_PASS() \
		printf("%d: %s() %s PASSED\n", __LINE__, __FUNCTION__, __FILE__);
#endif

extern int file_exist(const char *filename);
