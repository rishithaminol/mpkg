/*! @file info.c
 *	@brief Read info data from the package info file.
 *	@detail	This section contains info grammar and rules.
 *			Converts the given string into a struct type
 *			list. Using this we can keep multiple info
 *			objects in memory.
 *			options DEBUG___
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>

#include "mpkg.h"
#include "utils.h"
#include "ar.h"
#include "info.h"

/*! @brief infomation object */
typedef struct info_object {
	char *info_string;				/*! @brief string tobe converted */
	struct info_field *fld_list;	/*! @brief info_field after conversion */
} info_object;

/*!	@brief Maps every field name to pkg_fld_name
 *			and field type pkg_fld_type.
 */
static struct mav_rules {
	char		*str;	/*!< @brief mapped string. */
	pkg_fld_type fld_type;	/*!< @brief mapped pkg_fld_type. */
} fld_map[] = {
	{"package"			,fld_type_str},
	{"maintainer"		,fld_type_str},
	{"branch"			,fld_type_str},
	{"architecture"		,fld_type_str},
	{"version"			,fld_type_str},
	{"release"			,fld_type_str},
	{"installed-size"	,fld_type_int},
	{"dependancies"		,fld_type_str},
	{"homepage"			,fld_type_str},
	{"description"		,fld_type_str}
};

static char *_strsep(char **s1, const char *s2);
static pkg_fld_name info_fld_name(const char *key);
static struct info_field *info_allocate_fld(void);
static struct info_field *info_new_field(char *str);

/*!	@brief Behaves like strsep but matches entire \b s2. */
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

/*!	@brief Field name for the given key.
 *
 * Uses pre defined \b fld_map[] structure as a map.
 *
 * @param[in] key Key name to search.
 *
 * @return Return pkg_fld_name type for the given string.
 *			If error occured return \b fld_NULL.
 */
static pkg_fld_name info_fld_name(const char *key)
{
	/* number of elements */
	int n = (int)sizeof(fld_map)/sizeof(struct mav_rules);
	int i;

	for (i = 0; i < n; i++)
		if (strcmp(key, fld_map[i].str) == 0)
			return (pkg_fld_name)i;

	/* after the for loop raise warnings */
	fprintf(stderr, "%s: '%s' unrecognized field name\n", prog_name, key);

	/* not a mav field. */
	return fld_NULL;
}

/*! @brief malloc info_field */
static struct info_field *info_allocate_fld(void)
{
	return (struct info_field *)malloc(sizeof(struct info_field));
}

/*! @brief Convert to 'info_field *'.
 *
 *	\p str filter throug a regular expression. \p str
 *	separate into 2 parts as value and key.
 *	@param[in] str Any info file line.
 *
 *	@return return \b NULL on errors
 */
static struct info_field *info_new_field(char *str)
{
	regmatch_t pmatch[3];
	regex_t key_val_pair;
	struct info_field *info_fld;
	pkg_fld_name fld_name;

	char *key, *val;	/* key value pair dumy */

	regcomp_e_(&key_val_pair, "([a-z\\-]+) *: *\\\"(.*)\\\"");
	regexec_(&key_val_pair, str, 3, pmatch);

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
		info_fld->fld_type = fld_map[info_fld->fld_name].fld_type;

	switch (info_fld->fld_type) {
		case fld_type_int:
			info_fld->num = atoi(val);
			break;

		case fld_type_str:
			info_fld->str = val;
			break;

		case fld_type_nul:
			break;
	}

	info_fld->next = NULL;

	regfree(&key_val_pair);

	return info_fld;
}

/*! @brief convert to info_object type */
info_object *info_load(const char *str)
{
	char *t_str, *t_info_str;	/* dumy */
	struct info_field **t;		/* dumy */
	struct info_field *info_fields;
	info_object *obj = (info_object *)malloc(sizeof(info_object));

	obj->info_string = strdup(str);
	t_info_str = obj->info_string;
#ifdef DEBUG___
	printf("%s\n", obj->info_string);
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

	obj->fld_list = info_fields;

	return obj;
}

/*! @brief freeup loaded info_object */
void info_unload(info_object *obj)
{
	struct info_field *t, *curr_pos;

	t = obj->fld_list;

	while (1) {
		curr_pos = t;

		if (curr_pos == NULL)
			break;

		t = t->next;
		free(curr_pos);
	}

	free(obj->info_string);
	free(obj);
}

/*! @brief get the requested info_filed struct. */
struct info_field *info_get_fld(const info_object *iobj, pkg_fld_name name)
{
	struct info_field *t1; /* dumy */

	t1 = iobj->fld_list;

	while (t1 != NULL) {
		if (t1->fld_name == name)
			return t1;

		t1 = t1->next;
	}

	fprintf(stderr, "%s: '%s' is not in info object\n",
		prog_name, fld_map[name].str);

	return NULL;
}

/* "all,package,maintainer" */
/* options all, package, maintainer, architechture, version, dependancy, home,
 * release, description, installed-size
 */
void info_print(const info_object *info , const char *commands)
{
	char *s2, *s3, *s1;
	struct info_field *fld;
	pkg_fld_name name;

	s1 = strdup(commands);
	s3 = s1;

	while ((s2 = strsep(&s3, ",")) != NULL) {
		name = info_fld_name(s2);

		if (name == fld_NULL)
			break;

		fld = info_get_fld(info, info_fld_name(s2));
		if (fld->fld_type == fld_type_str)
			printf("%s: %s\n", s2, fld->str);
		else
			printf("%s: %d\n", s2, fld->num);
	}

	free(s1);
}
