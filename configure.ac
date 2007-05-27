# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.60)
AC_INIT([enter],[svn],[rob_dobbs@users.sourceforge.net])
AC_CONFIG_SRCDIR([src/enter.c])

AM_INIT_AUTOMAKE
AC_CONFIG_HEADERS([config.h])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O
AC_PROG_SED

# Checks for libraries.
AC_PATH_X
if test "$no_x" = "yes"; then
	AC_MSG_ERROR([Cannot find X Windows include files or libraries!])
fi

AC_PATH_PROGS([X_PATH], [X], [none])
if test "$X_PATH" = "none"; then
	AC_MSG_ERROR([Cannot find X Windows binary!])
fi

if test "x$x_includes" != "x"; then
	CFLAGS="$CFLAGS -I$x_includes";
fi
if test "x$x_libraries" != "x"; then
	LIBS="$LIBS -L$x_libraries";
fi

AC_CHECK_FT2

AC_CHECK_LIB([Xft], [XftFontOpenName], [LIBS="$LIBS -lXft"],
			[AC_MSG_ERROR([You need to have libXft.])])

AC_CHECK_LIB([Xau], [XauWriteAuth], [LIBS="$LIBS -lXau"],
			[AC_MSG_ERROR([You need to have libXau.])])

AC_CHECK_LIB([Imlib2], [imlib_context_new], [LIBS="$LIBS -lImlib2"],
			[AC_MSG_ERROR([You need to have Imlib2.])])

dnl AC_ARG_ENABLE([pam],
dnl	      [AS_HELP_STRING([--enable-pam], [enable pam authentication])],
dnl	      [pam=$enableval],
dnl	      [pam=no])

dnl if test "x$pam" = "xyes"; then
dnl	AC_CHECK_HEADERS([security/pam_appl.h], [],
dnl	      [AC_MSG_WARN([Could not find PAM headers, disabling PAM.])])
dnl fi

# Checks for header files.
AC_HEADER_STDC
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

DEBUG_CFLAGS="-Wall -pedantic -Wmissing-prototypes -Wmissing-declarations"
AC_SUBST([DEBUG_CFLAGS])
AC_SUBST([X_PATH])

AC_CONFIG_FILES([Makefile
                 data/Makefile
                 doc/Makefile
                 src/Makefile])
AC_OUTPUT

