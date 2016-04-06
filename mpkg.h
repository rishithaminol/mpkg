#ifndef MPKG_H
#define MPKG_H

#include "utils.h"

/* testing purpose */
#define ADMINISTRATIVE_DIR "."
#ifndef ADMINISTRATIVE_DIR
#define ADMINISTRATIVE_DIR "/var/lib/mpkg"
#endif

#ifndef MPKG_DB_
#define MPKG_DB_ path_append(ADMINISTRATIVE_DIR, "/mpkg.db")
#endif


#ifndef TMP_DIR
#define TMP_DIR "/tmp/mpkg"
#endif
#define TMP_DATA_DIR path_append(TMP_DIR, "data")
#define TMP_CONFIG_DIR path_append(TMP_DIR, "config")

extern char prefix[4096];
extern char *prog_name;
extern char *archive;

extern void mpkg_usage(int exit_status);

#endif /* MPKG_H */
