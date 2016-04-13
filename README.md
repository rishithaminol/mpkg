# Maavee mpkg package manager #

The Maavee mpkg package manager is a tool to manage '.mav' packages.
This tool is designed for run in Linux From Scratch (LFS)
systems. LFS systems does not have package managers that is why we
use this tool to manage packages under an LFS system. Currently this
program is in development.

### How do I get start? ###
This program is currently in development and still this program does not
have a proper build system. If someone needs to test this program. Type.

```
make mpkg
```
All other documentation can be obtained from doc/ directory.

### Contribution guidelines ###

* You can use your own debugging tools but I recommend
  valgrind and gdb to debug your executable(s).
* To beautify the code you can use uncrustify(The code
  beautifier for C). On some distributions this command does
  not exist. I recommend to use uncrustify.cfg as the default
  uncrustify config because it is a small derivative of
  uncrustify examples with a litle customizations.
* You can download the uncrustify source and compile it yourself.
  <http://sourceforge.net/projects/uncrustify/files/>.
* There are small scripts in the scripts/ directory you can
  use them to manage code yourself.
* And there are some example codes in directory examples/

### Coding style ###
Please folow linux kernel coding style
<https://www.kernel.org/doc/Documentation/CodingStyle>.
It is the most widely used coding style in C programming.

* you should enter new line at the end of every source file.

### Extras ###
* [Learn Markdown](https://bitbucket.org/tutorials/markdowndemo)

### Contacts ###

If someone wants to contact me please use <rishithaminol@gmail.com>