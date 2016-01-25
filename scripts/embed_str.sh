#!/bin/bash

echo "#include <stdio.h>
int main()
{
printf(\"\\"

while IFS='' read line; do
	echo "$line\\n\\" | sed 's/"/\\&/g'
done < "$1"

echo "\");
return 0;
}"
