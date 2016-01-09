#!/bin/bash

# compile
gcc -Wall -g -DDEBUG___ ../ar.c ar.c -o ar_test

./ar_test files/ncurses-5.9.mav
