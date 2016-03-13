CFLAGS=-Wall
CFLAGS+=-g
LDFLAGS=-lsqlite3

sqlfrms.o:
	$(CC) $(CFLAGS) -DDEBUG___ -DSQLITE_CUSTOM_DATABASE -DSQLITE_MPKG_DB="\"./mpkg.db\"" $(LDFLAGS) -c sqlfrms.c

mpkg: mpkg.o copy.o in_list.o utils.o ar.o info.o version.o v_compare.o sqlfrms.o

clean:
	scripts/del_executables.sh .
	scripts/del_objectfiles.sh .
