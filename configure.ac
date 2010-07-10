# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT([enter],[svn],[rob_dobbs@users.sourceforge.net])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_SRCDIR([src/enter.c])
AC_CONFIG_HEADER([config.h])
AC_CONFIG_AUX_DIR([build-tools])

AM_INIT_AUTOMAKE([dist-bzip2 no-dist-gzip])
AM_MAINTAINER_MODE([enable])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_SED
PKG_PROG_PKG_CONFIG

AC_PATH_PROGS([X_PATH], [X], [none])
if test "$X_PATH" = "none"; then
	AC_MSG_ERROR([Cannot find X Windows binary!])
fi
AC_SUBST([X_PATH])

# Checks for libraries.
PKG_CHECK_MODULES([XAU],[xau >= 1.0.5])
AC_SUBST(XAU_CFLAGS)
AC_SUBST(XAU_LIBS)

PKG_CHECK_MODULES([GTK],[gtk+-2.0 >= 2.20.1])
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

# Checks for header files.
AC_HEADER_SYS_WAIT

# Check for required headers.
AC_CHECK_HEADERS([stdlib.h string.h syslog.h unistd.h], [],
	[AC_MSG_ERROR([Required header not found.])])

# Check for optional headers.
AC_CHECK_HEADERS([shadow.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([endpwent getusershell memset strdup setenv getspnam \
	strerror atexit gethostname], [],
	[AC_MSG_ERROR([Required function not found.])])

AC_CHECK_LIB([crypt],[crypt])

DEBUG_CFLAGS="-Wall -pedantic -Wmissing-prototypes -Wmissing-declarations"
AC_SUBST([DEBUG_CFLAGS])

AC_OUTPUT([Makefile
	data/Makefile
	doc/Makefile
	src/Makefile])

