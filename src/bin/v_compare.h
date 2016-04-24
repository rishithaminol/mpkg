/*! @file v_compare.h
 *	@brief Version comparison scheme header.
 */

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
 */
typedef enum pkg_dep_type {
	dep_type_no,	/*!< @brief normal token */
	dep_type_or		/*!< @brief or token */
} pkg_dep_type;

/*! @brief package dependancy */
struct pkg_dep {
	char			*pkg_name;	/*!< @brief package name */
	am_binary		op;			/*!< @brief comparision operator */
	char			*v_num;		/*!< @brief version number */
	pkg_dep_type	dep_type;	/*!< @brief normal_token or OR_token */
	struct pkg_dep 	*next;
};

typedef struct dep_object dep_object;

extern dep_object *load_deps(const char *deps);
extern void unload_deps(dep_object *dobj);
