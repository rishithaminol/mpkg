#!/bin/bash

if (( $# < 1 )); then
	echo "error no arguments"
	exit 1
fi

find "$1" -name "*.o" | \
while read file_name; do
	rm -v "$file_name"
done
