/* output info_field structure chain and reduce
 * hard works for the version comparioshon scheme
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

#include "mpkg.h"
#include "ar.h"
#include "info.h"

static char *info_string;

static struct mav_rules {
	char	  		 *str;
	mav_fld_name fld_name;
	mav_fld_type fld_type;
} rule_table[] = {
	{"package"			,fld_pkg	,fld_type_str},
	{"maintainer"		,fld_maint	,fld_type_str},
	{"architecture"		,fld_arch	,fld_type_str},
	{"version"			,fld_ver	,fld_type_str},
	{"installed-size"	,fld_size	,fld_type_int},
	{"dependancies"		,fld_deps	,fld_type_dep},
	{"homepage"			,fld_home	,fld_type_str},
	{"description"		,fld_des	,fld_type_str},
};

static void _get_info(char *str, int n);
static char *_strsep(char **s1, const char *s2);
static am_binary _am_binary(const char *op);
static mav_fld_name info_fld_name(const char *key);
static mav_fld_type info_fld_type(const char *key);
static struct info_field *info_allocate_fld(void);
static struct info_field *info_new_field(char *str);
static struct mav_dep *info_allocate_dep(void);
static struct mav_dep *info_new_dep(char *str, mav_dep_type dep_type);
static struct mav_dep *info_get_deps(char *deps);
static void info_free_dep(struct mav_dep *deps);
static int _regcomp_e(regex_t *preg, const char *expr);

/**
 * fetch info data to the memory
 * utility used by ar_grab() in function info_load()
 */
static void _get_info(char *str, int n)
{
	static int count = 0;

	if (count > 0) {
		fprintf(stderr, "%s: info file size is larger than the default\n",
			prog_name);
		free(info_string);
		exit(EXIT_FAILURE);
	}

	info_string = strdup(str);
	count++;
}

/* behave like strsep but do not match all charachters in s2
 * matches entire s2
 */
static char *_strsep(char **s1, const char *s2)
{
	char *t1;
	char *t2 = *s1;

	t1 = strstr(*s1, s2);

	if (t1 == NULL)
		return NULL;

	*(t1 + 1) = '\0';
	*s1 = (t1 + 2);

	return t2;
}

/*
 * return enum type binary operator for
 * the given string type operator
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

/* return mav_fld_name for the given string */
static mav_fld_name info_fld_name(const char *key)
{
	/* number of elements */
	int n = (int)sizeof(rule_table)/sizeof(struct mav_rules);
	int i;

	for (i = 0; i < n; i++)
		if (strcmp(key, rule_table[i].str) == 0)
			return rule_table[i].fld_name;

	/* after the for loop raise warnings */
	fprintf(stderr, "%s: '%s' unrecognized field name\n", prog_name, key);

	/* if no name found. */
	return fld_NULL;
}

static mav_fld_type info_fld_type(const char *key)
{
	/* number of elements */
	int n = (int)sizeof(rule_table)/sizeof(struct mav_rules);
	int i;

	for (i = 0; i < n; i++)
		if (strcmp(key, rule_table[i].str) == 0)
			return rule_table[i].fld_type;

	return fld_type_nul;
}

static struct info_field *info_allocate_fld(void)
{
	return (struct info_field *)malloc(sizeof(struct info_field));
}

/* return NULL on errors */
static struct info_field *info_new_field(char *str)
{
	regmatch_t pmatch[3];
	regex_t key_val_pair;
	struct info_field *info_fld;
	mav_fld_name fld_name;

	char *key, *val;	/* key value pair dummies */

	_regcomp_e(&key_val_pair, "([a-z\\-]+) *: *\\\"(.*)\\\"");
	regexec(&key_val_pair, str, 3, pmatch, 0);

	str[pmatch[1].rm_eo] = '\0';
	str[pmatch[2].rm_eo] = '\0';

	key = str + pmatch[1].rm_so;
	val = str + pmatch[2].rm_so;

#ifdef DEBUG___
	/* print field name and value */
	printf("%s --> ", key);
	printf("%s\n", val);
#endif

	/* unrecognized field name */
	if ((fld_name = info_fld_name(key)) == fld_NULL) {
		regfree(&key_val_pair);
		return NULL;
	}

	info_fld = info_allocate_fld();
	info_fld->fld_name = fld_name;

	/* empty field recognition */
	if (strcmp(val, "") == 0)
		info_fld->fld_type = fld_type_nul;
	else
		info_fld->fld_type = info_fld_type(key);

	switch (info_fld->fld_type) {
		case fld_type_int:
			info_fld->num = atoi(val);
			break;

		case fld_type_str:
			info_fld->str = val;
			break;

		case fld_type_dep:
			info_fld->dep = info_get_deps(val);
			break;

		case fld_type_nul:
			break;
	}

	info_fld->next = NULL;

	regfree(&key_val_pair);

	return info_fld;
}

struct info_field *info_load(ar_object *obj)
{
	char *t_str;
	char *t_info_str;
	struct info_field *info_fields, **t;

	ar_grab(obj, "info", _get_info);
	t_info_str = info_string;
#ifdef DEBUG___
	printf("%s\n", info_string);
#endif

	t = &info_fields;

	while ((t_str = _strsep(&t_info_str, "\"\n")) != NULL) {
#ifdef DEBUG___
		printf(">>> %s\n", t_str);
#endif
		*t = info_new_field(t_str);

		if (*t == NULL)		/* on errors */
			continue;

		t = &((*t)->next);
	}

	return info_fields;
}

/*************** mav_dep structure handling ***************/

static struct mav_dep *info_allocate_dep(void)
{
	return (struct mav_dep *)malloc(sizeof(struct mav_dep));
}

static struct mav_dep *info_new_dep(char *str, mav_dep_type dep_type)
{
	struct mav_dep *mav_dep;
	char *pkg_name, *op, *v_num; /* dummies */

	regmatch_t pmatch[4];
	regex_t reg_ex;
	_regcomp_e(&reg_ex,
		"([a-z0-9]+)[ \\t]*\\(([<>=][<>=]*)[ \\t]*([0-9\\.]*)\\)");
	regexec(&reg_ex, str, 4, pmatch, 0);

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

	mav_dep = info_allocate_dep();
	mav_dep->pkg_name = pkg_name;
	mav_dep->op = _am_binary(op);
	mav_dep->v_num = v_num;
	mav_dep->dep_type = dep_type;
	mav_dep->next = NULL;

	regfree(&reg_ex);

	return mav_dep;
}

/* get dependancy list */
static struct mav_dep *info_get_deps(char *deps)
{
	char *t_str, *t_str1;
	struct mav_dep *mav_dep, **t;

	t = &mav_dep;

	while (!((t_str = strsep(&deps, ",")) == NULL)) {
		if (strchr(t_str, '|') == NULL) {
			*t = info_new_dep(t_str, dep_type_no);
			t = &((*t)->next);
		} else {
			while (!((t_str1 = strsep(&t_str, "|")) == NULL)) {
				*t = info_new_dep(t_str1, dep_type_or);
				t = &((*t)->next);
			}
		}
	}

	return mav_dep;
}

static void info_free_dep(struct mav_dep *deps)
{
	struct mav_dep *t, *curr_pos;

	t = deps;

	while (1) {
		curr_pos = t;

		if (curr_pos == NULL)
			break;

		t = t->next;
		free(curr_pos);
	}
}

void info_unload(struct info_field *flds)
{
	struct info_field *t, *curr_pos;

	t = flds;

	while (1) {
		curr_pos = t;

		if (curr_pos == NULL)
			break;

		if (curr_pos->fld_type == fld_type_dep)
			info_free_dep(curr_pos->dep);

		t = t->next;
		free(curr_pos);
	}

	free(info_string);
}

static int _regcomp_e(regex_t *preg, const char *expr)
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
