#!/bin/bash

# compile
gcc -Wall -g -c ../ar.c -o ar_test.o	#compile without DEBUG___
gcc -Wall -g -DDEBUG___ -I../json-c/include/json-c ../json-c/lib/libjson-c.a ar_test.o ../info.c info_test.c -o info_test

./info_test files/ncurses-5.9.mav
