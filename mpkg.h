#ifndef MPKG_H
#define MPKG_H

#ifndef MPKG_DB_
#define MPKG_DB_ "/var/lib/mpkg/mpkg.db"
#endif

#ifndef CUSTOM_TEMP_LOCATION
#define TMP_DIR "/tmp/mpkg"
#define TMP_DATA_DIR "/tmp/mpkg/data"
#define TMP_CONFIG_DIR "/tmp/mpkg/config"
#endif

extern char prefix[4096];
extern char *prog_name;
extern char *archive;

extern void mpkg_usage(int exit_status);

#endif /* MPKG_H */
