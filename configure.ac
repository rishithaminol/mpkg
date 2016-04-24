#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.69])
AC_INIT([mpkg], [1.0], [rishithaminol@gmail.com])
AC_CONFIG_SRCDIR([src/bin/mpkg_err.h])
AC_CONFIG_HEADERS([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_AWK
AC_PROG_CC_C99
AC_PROG_CC

# Checks for libraries.
# FIXME: Replace `main' with a function in `-lsqlite3':
# AC_CHECK_LIB([sqlite3], [main])
#AC_CHECK_LIB([sqlite3])
PKG_CHECK_MODULES([SQLITE3], [sqlite3], [have_libsqlite3=yes], [have_libsqlite3=no])
AM_CONDITIONAL([LIB_SQLITE3], [test "$have_libsqlite3" = "yes"])

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h stddef.h stdint.h stdlib.h string.h unistd.h sqlite3.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_HEADER_STDBOOL
AC_TYPE_UID_T
AC_TYPE_MODE_T
AC_TYPE_OFF_T
AC_TYPE_SSIZE_T

# Checks for library functions.
AC_FUNC_MALLOC
AC_FUNC_REALLOC
AC_CHECK_FUNCS([getcwd mkdir regcomp strchr strdup strerror strstr strtol])

# AC_CONFIG_FILES([Makefile
#                 examples/Makefile
#                 sqlite/Makefile
#                 src/bin/Makefile
#                 tests/Makefile])

AC_CONFIG_FILES([Makefile
                 src/bin/Makefile])
AC_OUTPUT
