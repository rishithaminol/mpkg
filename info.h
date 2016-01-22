#ifndef INFO_H
#define INFO_H

#include "ar.h"

/*! @brief arithmatic binary operators */
typedef enum am_binary {
	eq = 1,	/*!< = */
	lt,		/*!< < */
	le,		/*!< <= --> lt + eq */
	gt,		/*!< > */
	ge		/*!< >= --> gt + eq */
} am_binary;

/*! @detail There are two types of tokens \n
 * Normal token:						  \n
 * ", pkgname (>= 2.1.5),"				  \n
 * OR token:							  \n
 * ", pkgfoo (= 2.7.9) | pkgbar (>= 7.5.3),"
 *	@todo This section should be moved into a new version
 *		  comparison scheme.
 */
typedef enum mav_dep_type {
	dep_type_no,	/*!< @brief normal token */
	dep_type_or		/*!< @brief or token */
} mav_dep_type;

/*! @brief nava dependancy package */
struct mav_dep {
	char			*pkg_name;	/*!< @brief package name */
	am_binary		op;			/*!< @brief comparision operator */
	char			*v_num;		/*!< @brief version number */
	mav_dep_type	dep_type;	/*!< @brief normal_token or OR_token */
	struct mav_dep 	*next;
};

/*!	@brief Navam package filelds. */
typedef enum mav_fld_name {
	fld_NULL,	/*!< @brief null field (no field) */
	fld_pkg,	/*!< @brief package */
	fld_maint,	/*!< @brief maintainer */
	fld_arch,	/*!< @brief architecture */
	fld_ver,	/*!< @brief version */
	fld_size,	/*!< @brief installed-size */
	fld_deps,	/*!< @brief dependancies */
	fld_home,	/*!< @brief homepage info */
	fld_des		/*!< @brief description */
} mav_fld_name;

/*!	@brief field types */
typedef enum mav_fld_type {
	fld_type_nul,	/*!< @brief "" (no value) */
	fld_type_int,	/*!< @brief number */
	fld_type_str,	/*!< @brief string */
	fld_type_dep	/*!< @brief dependancy struct */
} mav_fld_type;

/*!	@brief to store field data */
struct info_field {
	mav_fld_name 	fld_name;		/*!< @brief field key */
	union {
		unsigned int num;			/*!< @brief number */
		char *str;					/*!< @brief string */
		struct mav_dep *dep;		/*!< @brief mav_dep type */
	};
	mav_fld_type 	fld_type;		/*!< @brief field type */
	struct info_field *next;
};

extern struct info_field *info_load(ar_object *obj);
extern void info_unload(struct info_field *flds);

#endif	/* INFO_H */
