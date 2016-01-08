JSON_C_DIR=json-c

CFLAGS=-Wall
CFLAGS+=-g
CFLAGS+=-I$(JSON_C_DIR)/include/json-c
LDFLAGS+=$(JSON_C_DIR)/lib/libjson-c.a

mpkg: mpkg.o copy.o in_list.o utils.o ar.o info.o

clean:
	scripts/del_executables.sh .
	scripts/del_objectfiles.sh .
