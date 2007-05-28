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
	list.c list.h \
	login.c login.h \
	display.c display.h \
	gui_image.c gui_image.h \
	gui_label.c gui_label.h \
	gui_input.c gui_input.h \
	gui_widget.c gui_widget.h \
	gui.c gui.h \
	greeter.c greeter.h

enter_CFLAGS = @FT2_CFLAGS@ -DCONFDIR=\"$(sysconfdir)\" -DTHEMEDIR=\"$(datadir)/@PACKAGE@/themes\"
enter_LDFLAGS = @FT2_LIBS@

