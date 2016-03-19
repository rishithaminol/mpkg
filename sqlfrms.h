#ifndef SQLFRMS_H
#define SQLFRMS_H

#include <sqlite3.h>

#ifndef SQLITE_CUSTOM_DATABASE
#define SQLITE_CUSTOM_DATABASE
#define MPKG_SQLITE_DB_DIR "/var/lib/mpkg"
#define SQLITE_MPKG_DB path_append(MPKG_SQLITE_DB_DIR, "mpkg.db")
#define SQLITE_MPKG_ALL_PKGS_DB path_append(MPKG_SQLITE_DB_DIR, "all_pkgs.db")
#endif

extern void update_db(sqlite3 *db, info_object *iobj, const char *install_log);
extern char *archive_name(sqlite3 *db, const char *name);
extern int get_pkgID(sqlite3 *db, const char *str);
extern int is_installed(sqlite3 *db, const char *str);
extern sqlite3 *open_main_db(void);
extern void close_main_db(sqlite3 *db);

#endif /* SQLFRMS_H */