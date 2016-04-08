#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

#include "mpkg.h"
#include "utils.h"

char p_app[65536];	/* path append string */

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

/*! @brief Appends two pathnames
 *
 *	Using \bp_app global variable this function append new strings
 *	which are ending at '\0' charachter and return the appended
 *	string's first address. Following example will print only
 *	"/etc/db".
 *	+---+---+----+---+---+---+---+---+---+---+----+---+---+
 *	| l | o | \0 | / | e | t | c | / | d | b | \0 | / | u |
 *	+---+---+----+---+---+---+---+---+---+---+----+---+---+
 *	               ^
 */
char *path_append(const char *p1, const char *p2)
{
	static int p_app_len = 0;
	int cur_len;	/*current len*/

	/* length of '/' + strlen(p1) + strlen(p2) + null charachter */
	cur_len = (strlen(p1) + strlen(p2) + 2);
	p_app_len += cur_len;

	sprintf(p_app + p_app_len - cur_len, "%s/%s", p1, p2);

	return p_app + p_app_len - cur_len;
}

/* @brief String appending
 * @param[in] str1 Source string.
 * @param[in] str2 String to be appended.
 * @todo This function will only work with malloced
 *			variables.
 */
void strappend(char **str1, const char *str2)
{
	*str1 = (char *)realloc(*str1, strlen(*str1) + strlen(str2) + 1);
	strcat(*str1, str2);
}
