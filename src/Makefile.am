AUTOMAKE_OPTIONS=nostdinc

AM_CPPFLAGS = -I..

sbin_PROGRAMS = enter

enter_SOURCES = \
	enter.c enter.h \
	server.c server.h \
	utils.c utils.h \
	conf.c conf.h \
	log.c log.h \
	display.c display.h \
	gui_image.c gui_image.h \
	gui_label.c gui_label.h \
	gui_input.c gui_input.h \
	gui.c gui.h \
	auth.c auth.h

enter_CFLAGS = @DEBUG_CFLAGS@ @FT2_CFLAGS@ @IMLIB_CFLAGS@ @X_CFLAGS@ -DCONFDIR=\"$(sysconfdir)\"
enter_LDFLAGS = @X_LIBS@ @IMLIB_LIBS@ @FT2_LIBS@ -lcrypt

