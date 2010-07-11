AUTOMAKE_OPTIONS=nostdinc

AM_CPPFLAGS = -I..

bin_PROGRAMS = enter

enter_SOURCES = \
	enter.c enter.h \
	server.c server.h \
	conf.c conf.h \
	auth.c auth.h \
	log.c log.h \
	login.c login.h

enter_CFLAGS = \
	@GTK_CFLAGS@ \
	@XAU_CFLAGS@ \
	-DCONFDIR=\"$(sysconfdir)\" -DTHEMEDIR=\"$(datadir)/@PACKAGE@/themes\"

enter_LDADD = \
	@GTK_LIBS@ \
	@XAU_LIBS@ -lcrypt

