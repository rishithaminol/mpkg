CC = gcc
CFLAGS = -Wall -g
DEBUG_FLAGS = -DDEBUG___
OBJECTS = ar.o info.o utils.o v_compare.o


info_test: $(OBJECTS)
	$(CC) $(CFLAGS) $(OBJECTS) info_test.c -o info_test

ar.o:
	$(CC) $(CFLAGS) -c ../ar.c

info.o:
	$(CC) $(CFLAGS) $(DEBUG_FLAGS) -c ../info.c

utils.o:
	$(CC) $(CFLAGS) -c ../utils.c

v_compare.o:
	$(CC) $(CFLAGS) -c ../v_compare.c

clean:
	../scripts/del_executables.sh .
	../scripts/del_objectfiles.sh .
