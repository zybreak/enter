AUTOMAKE_OPTIONS=nostdinc

bin_PROGRAMS = enter

enter_SOURCES = \
	main.c \
	../utils.c \
	../cfg.c \
	display.c display.h \
	theme.c theme.h \
	window.c window.h

# This should be removed if we choose to use imlib2 instead
EXTRA_DIST = \
	png.c \
	jpeg.c

enter_CFLAGS = @FT2_CFLAGS@ @X_CFLAGS@ -I..
enter_LDFLAGS = @FT2_LIBS@ @X_LIBS@ -lX11
