#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <regex.h>

#include "mpkg.h"

/*! @brief check the exsistance of a file.
 *  @return return 0 on success. Otherwise return
 *			\b errno occured.
 */
int file_exist(const char *filename)
{
	struct stat tmp_stat;
	int ret;

	if ((ret = stat(filename, &tmp_stat)) == -1)
		return errno;

	return ret;
}

/*! @brief Compile regex.
 *
 *	Compile given \b expr and store regex_t in \b preg. If
 *  error occured reports it immediately.
 *
 *	@param[out]	preg regex_t to store.
 *	@param[in]	expr regular expression.
 *
 *	@return return 0 on succses. Otherwise return
 *			\b EXIT_FAILURE.
 */
int regcomp_e_(regex_t *preg, const char *expr)
{
	int regc_ret;
	char errbuf[256];

	regc_ret = regcomp(preg, expr, REG_EXTENDED);

	if (regc_ret != 0) {
		regerror(regc_ret, preg, errbuf, 256);
		fprintf(stderr, "%s: %s\n", prog_name, errbuf);
		return EXIT_FAILURE;
	}

	return 0;
}

/*! @brief Matches given regex and string.
 *
 *	Matches given \b string with \b preg. If error occured
 *	reports it immediately.
 *
 *	@param[in]	preg Pointer to regex_t.
 *	@param[in]	string String to match.
 *	@param[out]	pmatch array of matches.
 *	@param[in]	nmatch Number of matches expected.
 *
 *	@return return 0 on succses. Otherwise return
 *			\b REG_NOMATCH.
 */
int regexec_(const regex_t *preg, const char *string,
	ssize_t nmatch, regmatch_t *pmatch)
{
	int regex_ret;

	regex_ret = regexec(preg, string, nmatch, pmatch, 0);
	if (regex_ret != 0) {
		fprintf(stderr, "%s: '%s' no match\n", prog_name, string);
		return REG_NOMATCH;
	}

	return 0;
}