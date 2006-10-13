# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT([enter],[0.0.1], [rob_dobbs@users.sourceforge.net])
AC_CONFIG_SRCDIR([src/utils.c])

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([src/config.h])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC

# Checks for libraries.
AC_CHECK_FT2

# Checks for header files.
AC_PATH_XTRA
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h syslog.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([memset strdup])

AC_OUTPUT([
	   Makefile
	   data/Makefile
	   src/Makefile
	   src/greeter/Makefile
	   src/daemon/Makefile
])
