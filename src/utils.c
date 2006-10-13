#include <ctype.h>
#include <syslog.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"

void* xmalloc(size_t size)
{
	void* p = malloc(size);
	if (!p) {
		syslog(LOG_EMERG,"Could not allocate memory.");
		exit(EXIT_FAILURE);
	}
	return p;
}

char* strtrim(char *str)
{
	char *begin=str;
	char *end = str+strlen(str);
	
	while (*str && isspace(*(str++)))
		begin = str;
	
	while (isspace(*(end-1)))
		end--;
	
	*end = '\0';

	return begin;
}
