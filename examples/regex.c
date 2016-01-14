#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

#include "../info.h"

/* return or_token list */
void idea_1(char *str, regex_t *or_token)
{
	char *t_str;
	regmatch_t match_offsets[4];

	while (!((t_str = strsep(&str, "|")) == NULL)) {
		printf("%s\n", t_str);
		regexec(or_token, t_str, 4, match_offsets, 0);
		t_str[match_offsets[1].rm_eo] = '\0';
		printf("\tpkg_name = \"%s\"\n", t_str + match_offsets[1].rm_so);
		t_str[match_offsets[2].rm_eo] = '\0';
		printf("\tam_binary = \"%s\"\n", t_str + match_offsets[2].rm_so);
		t_str[match_offsets[3].rm_eo] = '\0';
		printf("\tv_num = \"%s\"\n", t_str + match_offsets[3].rm_so);
	}
}

int main(void)
{
	char *deps = strdup("pkgname (>= 2.1.5),pkgfoo (= 2.7.9)|pkgbar(>= 7.5.3.1)|pkgfoo2(<= 2.3)");
	char *t_deps = deps;
	char *t_str;
	mav_depend_type token_type;

	regmatch_t match_offsets[4];
	regex_t n_token;
	regcomp(&n_token, "([a-z0-9]+).*\\(([<>=][<>=]*) ([0-9\\.]*)\\)", REG_EXTENDED);

	while (!((t_str = strsep(&t_deps, ",")) == NULL)) {
		if (strchr(t_str, '|') == NULL) {
			printf("%s\n", t_str);
			regexec(&n_token, t_str, 4, match_offsets, 0);
			t_str[match_offsets[1].rm_eo] = '\0';
			printf("\tpkg_name = \"%s\"\n", t_str + match_offsets[1].rm_so);
			t_str[match_offsets[2].rm_eo] = '\0';
			printf("\tam_binary = \"%s\"\n", t_str + match_offsets[2].rm_so);
			t_str[match_offsets[3].rm_eo] = '\0';
			printf("\tv_num = \"%s\"\n", t_str + match_offsets[3].rm_so);
		} else {
			idea_1(t_str, &n_token);
		}
	}

	free(deps);
	regfree(&n_token);

	return 0;
}
