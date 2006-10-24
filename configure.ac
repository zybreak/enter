# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT([enter],[0.0.1], [rob_dobbs@users.sourceforge.net])
AC_CONFIG_SRCDIR([src/enter.c])

AM_INIT_AUTOMAKE
AM_CONFIG_HEADER([config.h])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
AC_CHECK_FT2

# Checks for header files.
AC_PATH_X
if test x"$no_x" != x; then
	AC_MSG_ERROR([Cannot find X Windows include files or libraries!])
fi

if test x"$x_includes" == x; then
	X_CFLAGS=""
else
	X_CFLAGS="-I$x_includes";
fi
if test x"$x_includes" == x; then
	X_LIBS=""
else
	X_LIBS="-L$x_libraries";
fi
AC_SUBST([X_CFLAGS])
AC_SUBST([X_LIBS])

dnl TODO: Add better check for libpng.
AC_SUBST([PNG_CFLAGS],[`libpng-config --cflags`])
AC_SUBST([PNG_LIBS],[`libpng-config --libs`])

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
])
