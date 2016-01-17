#ifndef INFO_H
#define INFO_H

#include "ar.h"

typedef enum mav_fld_name {	/* mavee package filelds */
	fld_NULL,	/* null field */
	fld_pkg,
	fld_maint,	/* maintainer */
	fld_arch,	/* architecture */
	fld_ver,	/* version */
	fld_size,	/* installed size */
	fld_deps,	/* dependancies */
	fld_home,	/* homepage info */
	fld_des		/* description */
} mav_fld_name;

/* arithmatic binary operators */
typedef enum am_binary {
	eq = 1,	/* = */
	lt,		/* < */
	le,		/* <= --> lt + eq */
	gt,		/* > */
	ge		/* >= --> gt + eq */
} am_binary;

/* There are two types of tokens
 * Normal token:
 * ", pkgname (>= 2.1.5),"
 * OR token:
 * ", pkgfoo (= 2.7.9) | pkgbar (>= 7.5.3),"
 */
typedef enum mav_dep_type {
	dep_type_no,	/* normal token */
	dep_type_or		/* or token */
} mav_dep_type;

struct mav_dep {
	char			*pkg_name;
	am_binary		op;			/* comparison operator */
	char			*v_num;		/* version number */
	mav_dep_type	dep_type;		/* normal_token or OR_token */
	struct mav_dep 	*next;
};

typedef enum mav_fld_type { /* field type */
	fld_type_nul,	/* "" --> no value */
	fld_type_int,	/* number */
	fld_type_str,	/* string */
	fld_type_dep	/* dependancy struct */
} mav_fld_type;

/* Main structure */
struct info_field {
	mav_fld_name 	fld_name;		/* field */
	union {							/* value */
		unsigned int num;
		char *str;
		struct mav_dep *dep;
	};
	mav_fld_type 	fld_type;		/* field type */
	struct info_field *next;
};

extern struct info_field *info_load(ar_object *obj);
extern void info_unload(struct info_field *flds);

#endif	/* INFO_H */
