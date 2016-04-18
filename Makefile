CFLAGS=-Wall
CFLAGS+=-g
LDFLAGS=-lsqlite3

sqlfrms.o:
	$(CC) $(CFLAGS) -DSQLITE_CUSTOM_DATABASE -DMPKG_SQLITE_DB_DIR="\".\"" $(LDFLAGS) -c sqlfrms.c

mpkg: mpkg.o copy.o in_list.o utils.o ar.o info.o version.o v_compare.o sqlfrms.o mpkg_err.o

clean:
	scripts/del_executables.sh .
	scripts/del_objectfiles.sh .
