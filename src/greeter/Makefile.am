AUTOMAKE_OPTIONS=nostdinc

bin_PROGRAMS = enter

enter_SOURCES = \
	main.c \
	../utils.c \
	../cfg.c \
	display.c display.h \
	png.c \
	window.c window.h

# This should be removed if we choose to use imlib2 instead
EXTRA_DIST = \
	jpeg.c

enter_CFLAGS = @FT2_CFLAGS@ @PNG_CFLAGS@ @X_CFLAGS@ -I..
enter_LDFLAGS = @FT2_LIBS@ @PNG_LIBS@ @X_LIBS@ -lX11
