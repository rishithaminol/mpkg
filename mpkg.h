#ifndef MPKG_H
#define MPKG_H

#ifndef MPKG_DB_
#define MPKG_DB_ "/var/lib/mpkg/mpkg.db"
#endif

extern char *tmp_dir;
extern char *tmp_data_dir;
extern char *tmp_config_dir;
extern char prefix[4096];
extern char *prog_name;
extern char *archive;

extern void mpkg_usage(int exit_status);

#endif /* MPKG_H */
