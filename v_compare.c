/*! @file v_compare.c
 *	@brief Version comparison scheme.
 *	@detail Has an ability to fetch infomation about installed packages.
 *			Search dependancies recursively.
 *			Gives attention on 'dependandcy' field only.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

#include "mpkg.h"
#include "utils.h"
#include "v_compare.h"

/*! @brief dependancy object */
typedef struct dep_object {
	char 		*dep_str;
	struct pkg_dep *dep_list;
} dep_object;


/*! @brief Convert string type binary operator.
 *	@param[in] op string type binary operator
 *		eg:- '<=', '=', '<=', ...
 */
static am_binary _am_binary(const char *op)
{
	am_binary operator;

	switch (*op) {
		case '=':	operator = eq; break;
		case '<':	operator = lt; break;
		case '>':	operator = gt; break;
	}

	switch (*(op + 1)) {
		case '=':	operator += eq; break;
		case '<':	operator += lt; break;
		case '>':	operator += gt; break;
		case '\0':	break;
	}

	return operator;
}	

/*! @brief malloc pkg_dep */
static struct pkg_dep *allocate_dep(void)
{
	return (struct pkg_dep *)malloc(sizeof(struct pkg_dep));
}

/*! @brief new dependancy */
static struct pkg_dep *new_dep(char *str, pkg_dep_type dep_type)
{
	struct pkg_dep *pkg_dep;
	char *pkg_name, *op, *v_num; /* dumy variables */

	regmatch_t pmatch[4];
	regex_t reg_ex;
	regcomp_e_(&reg_ex,
		"([a-z0-9]+)[ \\t]*\\(([<>=][<>=]*)[ \\t]*([0-9\\.]*)\\)");
	regexec_(&reg_ex, str, 4, pmatch);

	str[pmatch[1].rm_eo] = '\0';
	str[pmatch[2].rm_eo] = '\0';
	str[pmatch[3].rm_eo] = '\0';

	pkg_name = str + pmatch[1].rm_so;
	op = str + pmatch[2].rm_so;
	v_num = str + pmatch[3].rm_so;

#ifdef DEBUG___
	printf("\tpkg_name = \"%s\"\n", pkg_name);
	printf("\tam_binary = \"%s\"\n", op);
	printf("\tv_num = \"%s\"\n", v_num);
#endif

	pkg_dep = allocate_dep();
	pkg_dep->pkg_name = pkg_name;
	pkg_dep->op = _am_binary(op);
	pkg_dep->v_num = v_num;
	pkg_dep->dep_type = dep_type;
	pkg_dep->next = NULL;

	regfree(&reg_ex);

	return pkg_dep;
}

/*!	@brief dependancy list
 *
 *	Read \b deps and breakes into tokens. These tokens are
 *	dependancy tokens. They are categoraized into two parts.
 *	normal dependancy and not very important.
 *	@param[in]	deps dependancy string.
 */
dep_object *load_deps(const char *deps)
{
	char *s1, *s2, *s3;	/* dumy */
	struct pkg_dep **t;		/* dumy */
	struct pkg_dep *pkg_dep_list;
	dep_object	*dobj = (dep_object *)malloc(sizeof(dep_object));

	dobj->dep_str = strdup(deps);
	s3 = dobj->dep_str;

	t = &pkg_dep_list;
	while (!((s1 = strsep(&s3, ",")) == NULL)) {
		if (strchr(s1, '|') == NULL) {
			*t = new_dep(s1, dep_type_no);
			t = &((*t)->next);
		} else {
			while (!((s2 = strsep(&s1, "|")) == NULL)) {
				*t = new_dep(s2, dep_type_or);
				t = &((*t)->next);
			}
		}
	}

	dobj->dep_list = pkg_dep_list;

	return dobj;
}

void unload_deps(dep_object *dobj)
{
	struct pkg_dep *t, *curr_pos;

	t = dobj->dep_list;
	while (1) {
		curr_pos = t;

		if (curr_pos == NULL)
			break;

		t = t->next;
		free(curr_pos);
	}

	free(dobj->dep_str);
	free(dobj);
}
