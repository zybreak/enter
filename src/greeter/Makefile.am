AUTOMAKE_OPTIONS=nostdinc

bin_PROGRAMS = enter

enter_SOURCES = \
	main.c \
	../utils.c \
	../cfg.c \
	display.c display.h \
	image.c image.h \
	theme.c theme.h \
	window.c window.h

enter_CFLAGS = @FT2_CFLAGS@ @PNG_CFLAGS@ @X_CFLAGS@ -I..
enter_LDFLAGS = @FT2_LIBS@ @PNG_LIBS@ @X_LIBS@ -lX11 -lXft
