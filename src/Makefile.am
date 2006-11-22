AUTOMAKE_OPTIONS=nostdinc

AM_CPPFLAGS = -I..

noinst_LIBRARIES = libcommon.a

sbin_PROGRAMS = enter enter_greeter

libcommon_a_SOURCES = \
	utils.c \
	utils.h \
	conf.c \
	conf.h

enter_SOURCES = \
	enter.c enter.h \
	enter_greeter.c enter_greeter.h \
	enter_server.c enter_server.h

enter_greeter_SOURCES = \
	greeter.c greeter.h \
	greeter_display.c greeter_display.h \
	greeter_image.c greeter_image.h \
	greeter_gui_widgets.c greeter_gui_widgets.h \
	greeter_gui.c greeter_gui.h

enter_CFLAGS = @DEBUG_CFLAGS@ @X_CFLAGS@ -DCONFDIR=\"$(sysconfdir)\"
enter_LDFLAGS = @X_LIBS@
enter_LDADD = libcommon.a

enter_greeter_CFLAGS = @DEBUG_CFLAGS@ @FT2_CFLAGS@ @IMLIB_CFLAGS@ @X_CFLAGS@
enter_greeter_LDFLAGS = @X_LIBS@ @IMLIB_LIBS@ @FT2_LIBS@
enter_greeter_LDADD = libcommon.a

