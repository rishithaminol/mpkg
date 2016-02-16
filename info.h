#ifndef INFO_H
#define INFO_H

#include "ar.h"

/*!	@brief Navam package filelds. */
typedef enum pkg_fld_name {
	fld_pkg,	/*!< @brief package */
	fld_maint,	/*!< @brief maintainer */
	fld_branch,	/*!< @brief branch */
	fld_arch,	/*!< @brief architecture */
	fld_ver,	/*!< @brief version */
	fld_rele,	/*!< @brief release */
	fld_size,	/*!< @brief installed-size */
	fld_deps,	/*!< @brief dependancies */
	fld_home,	/*!< @brief homepage info */
	fld_des,	/*!< @brief description */
	fld_NULL	/*!< @brief null field (no field) */
} pkg_fld_name;

/*!	@brief field types */
typedef enum pkg_fld_type {
	fld_type_nul,	/*!< @brief "" (no value) */
	fld_type_int,	/*!< @brief number */
	fld_type_str,	/*!< @brief string */
} pkg_fld_type;

/*!	@brief to store field data */
struct info_field {
	pkg_fld_name 	fld_name;		/*!< @brief field key */
	union {
		unsigned int num;			/*!< @brief number */
		char *str;					/*!< @brief string */
	};
	pkg_fld_type 	fld_type;		/*!< @brief field type */
	struct info_field *next;
};

typedef struct info_object info_object;

info_object *info_load(const char *str);
extern void info_unload(info_object *iobj);
extern struct info_field *info_get_fld(const info_object *iobj,
	pkg_fld_name name);
extern void info_print(const info_object *info ,
	const char *commands);

#endif	/* INFO_H */
