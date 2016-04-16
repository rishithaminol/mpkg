/*! @file sqlfrms.c
 *	@brief Predefined SQL frames to perform database
 *			transactions.
 *	@detail This section should know pre-defined
 *			Database location macores as well.
 *			It simply says that this section knows the pathnames of db
 *			locations.
 *	@todo There should be a function to determine
 *			<<OS_codename>>-<<branch>> type database
 *			types and tables.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

#include "utils.h"
#include "info.h"
#include "sqlfrms.h"
#include "v_compare.h"	/* pkg_dep structure */

int get_pkgID(sqlite3 *db, const char *str);

/* @brief create archive name from the data.
 *
 * This is the archive file name on the server.
 *
 * @param[in] name create the package name by using
 *					this.
 * @param[in] db Opened sqlite3 database.
 * @return	Return the created string.
 * @todo Use get_pkgID() as a subtitute for \bname
 */
char *archive_name(sqlite3 *db, const char *name)
{
	sqlite3_stmt *res_1, *res_2;
	char *sql_1, *sql_2;
	int pkgID;
	char *pkgName, *pkgBranch, *pkgVer, *pkgRelease, *pkgArch;
	char *s1 = (char *)malloc(256);

	sql_1 = sqlite3_mprintf(
		"SELECT pkgID, pkgName, pkgBranch FROM all_pkgs WHERE pkgName = '%s'",
		name);

	if (sqlite3_prepare_v2(db, sql_1, -1, &res_1, 0) != SQLITE_OK) {
		mpkg_err("can't retraive 1 data: %s\n", sqlite3_errmsg(db));
		return NULL;
	}

	if (sqlite3_step(res_1) == SQLITE_ROW) {
		pkgBranch = (char *)sqlite3_column_text(res_1, 2);
		pkgName = (char *)sqlite3_column_text(res_1, 1);
		pkgID = sqlite3_column_int(res_1, 0);
	} else {	/* if no name found */
		sqlite3_free(sql_1);
		free(s1);
		sqlite3_finalize(res_1);
		return NULL;
	}

	sql_2 = sqlite3_mprintf(
		"SELECT pkgID, pkgArch, pkgVer, pkgRelease FROM %s_%s\
		WHERE pkgArch = '%s' AND pkgID = %d",
		OS_CODENAME, pkgBranch, SYS_ARCH, pkgID);

	if (sqlite3_prepare_v2(db, sql_2, -1, &res_2, 0) != SQLITE_OK) {
		mpkg_err("can't retraive 2 data: %s\n", sqlite3_errmsg(db));
		return NULL;
	}

	if (sqlite3_step(res_2) == SQLITE_ROW) {
		pkgVer = (char *)sqlite3_column_text(res_2, 2);
		pkgRelease = (char *)sqlite3_column_text(res_2, 3);
		pkgArch = (char *)sqlite3_column_text(res_2, 1);
	}

	sprintf(s1, "%s-%s-%s.%s.mav", pkgName, pkgVer, pkgRelease, pkgArch);
	s1 = realloc(s1, strlen(s1) + 1);

	sqlite3_free(sql_1);
	sqlite3_free(sql_2);
	sqlite3_finalize(res_2);
	sqlite3_finalize(res_1);

	return s1;
}

int get_pkgID(sqlite3 *db, const char *str)
{
	char *sql_1;
	sqlite3_stmt *res_1;
	int pkg_id;

	sql_1 = sqlite3_mprintf("SELECT pkgID, pkgName FROM all_pkgs WHERE pkgName = '%s'",
		str);

	if (sqlite3_prepare_v2(db, sql_1, -1, &res_1, 0) != SQLITE_OK) {
		mpkg_err("can't retraive data %s\n", sqlite3_errmsg(db));
		sqlite3_free(sql_1);
		return -1;
	}

	if (sqlite3_step(res_1) == SQLITE_ROW) {
		pkg_id = sqlite3_column_int(res_1, 0);
	} else {
		mpkg_err("Sorry, we don't know what '%s' is.\n", str);
		sqlite3_free(sql_1);
		sqlite3_finalize(res_1);
		return -1;
	}

	sqlite3_free(sql_1);
	sqlite3_finalize(res_1);

	return pkg_id;
}

/* @brief Checks whether package named 'str' is installed.
 * @param[in] db Opened sqlite3 database.
 * @param[in] str Package name to check.
 * @return returns -1 on errors otherwise return rowid of
 * 					installed once.
 */
int is_installed(sqlite3 *db, const char *str)
{
	sqlite3_stmt *res_2;
	char *sql_2;
	int pkg_id, index;

	pkg_id = get_pkgID(db, str);

	sql_2 = sqlite3_mprintf("SELECT index_, pkgID FROM installed WHERE pkgID = '%d'",
		pkg_id);

	if (sqlite3_prepare_v2(db, sql_2, -1, &res_2, 0) != SQLITE_OK) {
		mpkg_err("can't retraive 2 data: %s\n", sqlite3_errmsg(db));
		return -1;
	}

	if (sqlite3_step(res_2) == SQLITE_ROW)
		index = sqlite3_column_int(res_2, 0);
	else
		index = -1;

	sqlite3_free(sql_2);
	sqlite3_finalize(res_2);

	return index;
}

/* @brief Update the database using \b iobj.
 * @param[in] db Opened sqlite3 database.
 * @param[in] iobj Info object.
 */
void update_db(sqlite3 *db, info_object *iobj, const char *install_log)
{
	char pkg_id[10];
	char *sql_final, *sql_data;
	struct info_field *info_field;
	char *sql_err_msg;

	struct column_map {
		pkg_fld_name fld_name;
		char 		 *columnID;
	} column_map[] = {
		{fld_maint, "pkgMaint"},
		{fld_arch,  "pkgArch"},
		{fld_ver,   "pkgVer"},
		{fld_deps,  "pkgDep"},
		{fld_home,  "pkgHome"},
		{fld_rele,  "pkgRelease"},
		{fld_des,   "pkgDes"},
		{fld_size,  "installedSize"},
		{fld_NULL,  NULL}
	};

	info_field = info_get_fld(iobj, fld_pkg);
	sprintf(pkg_id, "%d", get_pkgID(db, info_field->str));

	sql_final = strdup("INSERT INTO installed(");
	strappend(&sql_final, "pkgID, ");
	sql_data = strdup(" VALUES(");
	strappend(&sql_data, pkg_id);
	strappend(&sql_data, ", ");

	int i;
	for (i = 0; column_map[i].fld_name != fld_NULL; i++) {
		info_field = info_get_fld(iobj, column_map[i].fld_name);
		strappend(&sql_final, column_map[i].columnID);
		strappend(&sql_data, "'");
		strappend(&sql_data, info_field->str);
		strappend(&sql_data, "'");

		if (column_map[i + 1].fld_name != fld_NULL) {
			strappend(&sql_final, ", ");
			strappend(&sql_data, ", ");
		}
	}

	/* installLog */
	strappend(&sql_final, ", installLog");
	strappend(&sql_data, ", '");
	strappend(&sql_data, install_log);
	strappend(&sql_data, "'");

	strappend(&sql_final, ")");
	strappend(&sql_data, ")");

	strappend(&sql_final, sql_data);

	/* update the database */
	int rc = sqlite3_exec(db, sql_final, 0, 0, &sql_err_msg);
	if (rc != SQLITE_OK) {
		mpkg_warn("%s\n", sql_err_msg);
		sqlite3_free(sql_err_msg);
	}

#ifdef DEBUG___
	PRINTF_STRING(sql_final);
#endif

	free(sql_final);
	free(sql_data);
}

/*
 *@brief open the main database
 */
sqlite3 *open_main_db(void)
{
	sqlite3 *db;

	if (sqlite3_open(SQLITE_MPKG_DB, &db) != SQLITE_OK) {
		sqlite3_close(db);
		mpkg_err("can't open database %s\n", sqlite3_errmsg(db));
		return NULL;
	}

	return db;
}

/* @brief close the opened main database
 * @todo need more attention to error checking.
 *		 should check whether the database fully destroyed
 */
void close_main_db(sqlite3 *db)
{
	sqlite3_close(db);
}
