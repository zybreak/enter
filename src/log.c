#include <stdio.h>
#include <stdarg.h>

#include "enter.h"
#include "log.h"

#define MSG_LEN 256

static int daemon = FALSE;

void log_daemon(int _daemon)
{
	daemon = _daemon;
}

void log_print(int level, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	if (!daemon) {
		vfprintf(stderr, format, args);
		fprintf(stderr,"\n");
	}

	vsyslog(level, format, args);
	
	va_end(args);
}
