#ifndef MPKG_H
#define MPKG_H

#ifndef MPKG_DB_
#define MPKG_DB_ "/var/lib/mpkg/mpkg.db"
#endif

#include "utils.h"

#ifndef CUSTOM_TEMP_LOCATION
#define TMP_DIR "/tmp/mpkg"
#define TMP_DATA_DIR path_append(TMP_DIR, "data")
#define TMP_CONFIG_DIR path_append(TMP_DIR, "config")
#endif

extern char prefix[4096];
extern char *prog_name;
extern char *archive;

extern void mpkg_usage(int exit_status);

#endif /* MPKG_H */
