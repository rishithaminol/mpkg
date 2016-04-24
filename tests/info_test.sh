#!/bin/bash

# compile
gcc -Wall -g -c ../src/ar.c	#compile without DEBUG___
gcc -Wall -g -c ../src/info.c
gcc -Wall -g -DDEBUG___ ar.o info.o info_test.c -o info_test

./info_test files/ncurses-5.9.mav
