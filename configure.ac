# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT([enter],[0.0.4],[rob_dobbs@users.sourceforge.net])
AC_CONFIG_SRCDIR([src/enter.c])

AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_RANLIB

# Checks for libraries.
AC_PATH_X
if test x"$no_x" = x"yes"; then
	AC_MSG_ERROR([Cannot find X Windows include files or libraries!])
fi

AC_PATH_PROGS([X_PATH], [X], [none])
if test "x$X_PATH" = "xnone"; then
	AC_MSG_ERROR([Cannot find X Windows binary!])
fi
AC_SUBST([X_PATH])

if test x"$x_includes" != x; then
	X_CFLAGS="-I$x_includes";
fi
if test x"$x_libraries" != x; then
	X_LIBS="-L$x_libraries";
fi

AC_CHECK_FT2

AC_PATH_PROGS([XFT_CONFIG], [xft-config], [none])
if test "x$XFT_CONFIG" = "xnone"; then
	AC_MSG_ERROR([You need to have libXft.])
fi
XFT_LIBS=`$XFT_CONFIG --libs`
AC_CHECK_LIB([Xft], [XftFontOpenName],,
			AC_MSG_ERROR([You need to have libXft.]),
			$XFT_LIBS)

AC_CHECK_LIB([Xau], [XauWriteAuth],,
			AC_MSG_ERROR([You need to have libXau.]))

AC_PATH_PROGS([IMLIB_CONFIG], [imlib2-config], [none])
if test "x$IMLIB_CONFIG" = "xnone"; then
	AC_MSG_ERROR([You need to have Imlib2.])
fi
IMLIB_LIBS=`$IMLIB_CONFIG --libs`

AC_CHECK_LIB([Imlib2], [imlib_context_new],,
			AC_MSG_ERROR([You need to have Imlib2.]),
			$IMLIB_LIBS)

# Checks for header files.
AC_SUBST([X_CFLAGS])
AC_SUBST([X_LIBS])

AC_SUBST([IMLIB_CFLAGS],[])
AC_SUBST([IMLIB_LIBS],[-lImlib2])

AC_HEADER_STDC
AC_HEADER_SYS_WAIT
AC_CHECK_HEADERS([stdlib.h string.h syslog.h unistd.h shadow.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_FORK
AC_FUNC_MALLOC
AC_TYPE_SIGNAL
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([endpwent getusershell memset strdup setenv getspnam strerror atexit])

DEBUG_CFLAGS="-Wall -Wmissing-prototypes -Wmissing-declarations"
AC_SUBST([DEBUG_CFLAGS])

AC_CONFIG_FILES([Makefile
                 data/Makefile
                 src/Makefile])
AC_OUTPUT
