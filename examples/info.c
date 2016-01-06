/* info file reader */

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <json.h>

#include "../utils.h"

/*
*** example mav info ***
{
	"package": "ncurses",
	"maintainer": "rishithaminol@gmail.com",
	"architecture": "amd64",
	"version": 5.9,
	"installed-size": 9250122,
	"dependancies": {
		"glibc": {"=>": 2.21},
		"binutils": {"=": 2.25}
		},
	"homepage": "www.maavee.org",
	"description": "ncurses (new curses) is a programming library
	providing an  API that allows the programmer to write text-based
	user interfaces in a terminal-independent manner. It is a toolkit
	for developing \"GUI-like\" application software that runs under
	a terminal emulator. It also optimizes screen changes,in order
	to reduce the latency experienced when using remote shells."
}
*/

/*
+----------------+-----------+
| Field          | JSON Type |
+----------------+-----------+
| <!MAV>		 | object    |
| package        | string    |
| maintainer     | string    |
| architechture  | string    |
| version        | int       |
| installed-size | int       |
| dependancies   | object    |
| homepage       | string    |
| description    | string    |
+----------------+-----------+
*/

int main(int argc, char *argv[]) {
	int fd;
	ssize_t n;
	char string[READ_BUFF];
	struct json_object *jobj;
	enum json_type type;

	fd = open(argv[1], O_RDONLY);
	n = read(fd, string, READ_BUFF);

	jobj = json_tokener_parse(string);

	{ /* preserve 'char *key' and 'struct json_object *val' */
	json_object_object_foreach(jobj, key, val) {
		type = json_object_get_type(val);

		switch (type) {
			case json_type_null: printf("json_type_null\n");
			break;
			case json_type_boolean: printf("json_type_boolean\n");
			break;
			case json_type_double: printf("json_type_double\n");
			break;
			case json_type_int: printf("json_type_int\n");
			break;
			case json_type_object: printf("json_type_object\n");
			break;
			case json_type_array: printf("json_type_array\n");
			break;
			case json_type_string: printf("json_type_string\n");
			break;
		}
	}
	}

	json_object_put(jobj);

	return 0;
}