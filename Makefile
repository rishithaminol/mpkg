JSON_C_DIR=json-c

CFLAGS=-Wall
CFLAGS+=-g

mpkg: mpkg.o copy.o in_list.o utils.o ar.o info.o version.o

clean:
	scripts/del_executables.sh .
	scripts/del_objectfiles.sh .
