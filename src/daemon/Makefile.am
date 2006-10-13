AUTOMAKE_OPTIONS=nostdinc

sbin_PROGRAMS = enterd

enterd_SOURCES = \
	greeter.c greeter.h \
	main.c \
	server.c server.h \
	../cfg.c \
	../utils.c

enterd_CFLAGS = -I.. @X_CFLAGS@
enterd_LDFLAGS = @X_LIBS@ -lX11
