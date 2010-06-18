AUTOMAKE_OPTIONS=nostdinc

AM_CPPFLAGS = -I..

bin_PROGRAMS = enter

enter_SOURCES = \
	enter.c enter.h \
	server.c server.h \
	utils.c utils.h \
	conf.c conf.h \
	auth.c auth.h \
	log.c log.h \
	login.c login.h \
	display.c display.h \
	gui_image.c gui_image.h \
	gui_label.c gui_label.h \
	gui_input.c gui_input.h \
	gui_widget.c gui_widget.h \
	gui.c gui.h \
	greeter.c greeter.h

enter_CFLAGS = \
	@IMLIB2_CFLAGS@ @XCB_CFLAGS@ \
	@XAU_CFLAGS@ @GLIB_CFLAGS@ \
	-DCONFDIR=\"$(sysconfdir)\" -DTHEMEDIR=\"$(datadir)/@PACKAGE@/themes\"

enter_LDFLAGS = \
	@IMLIB2_LIBS@ @XCB_LIBS@ \
	@XAU_LIBS@ @GLIB_LIBS@ -lcrypt

