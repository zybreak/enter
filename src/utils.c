#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "log.h"
#include "utils.h"

void* xmalloc(size_t size)
{
	void* p = malloc(size);
	if (!p) {
		log_print(LOG_EMERG,"Could not allocate memory.");
		exit(EXIT_FAILURE);
	}
	return p;
}

char* xstrtrim(char *str)
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

char* xstrcat(const char *str1, const char *str2)
{
	int size = strlen(str1)+strlen(str2);
	char *p = xmalloc(size+1);

	strncpy(p, str1, size);
	strncat(p, str2, size);

	return p;
}

void xstrdel(char *str, size_t pos)
{
	size_t i;
	for (i=1; i <= strlen(str); i++) {
		if (i > pos)
			str[i-1] = str[i];
	}
}

void xstrins(char *str, size_t pos, char c, size_t size)
{
}
