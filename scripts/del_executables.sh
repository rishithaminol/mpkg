#!/bin/bash

if (( $# < 1 )); then
	echo "error no arguments"
	exit 1
fi

find "$1" -type f ! -path "*/json-c/*" ! -path "*/tests/root*" -perm /u=x,g=x,o=x | \
while read file_name; do
	string=$(file "$file_name" | grep 'ASCII text')

	if [ -z "$string" ]; then
		rm -v "$file_name"
	fi
done
