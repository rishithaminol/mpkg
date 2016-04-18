#!/bin/bash

# compile
gcc -Wall -g -DDEBUG___ -c ../mpkg_err.c
gcc -Wall -g -DDEBUG___ -c ../ar.c
gcc -Wall -g -c ar_test.c
gcc -Wall -g mpkg_err.o ar.o ar_test.o -o ./ar_test

./ar_test files/ncurses-5.9-1.0.1.amd64.mav
