#ifndef __LOG_H__
#define __LOG_H__

#include <syslog.h>

void log_daemon(int _daemon);
void log_print(int level, const char *format, ...);

#endif
