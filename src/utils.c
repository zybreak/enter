#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "enter.h"

#include "log.h"
#include "utils.h"

void xstrdel(char *str, int pos)
{
	size_t i;
	for (i=1; i <= strlen(str); i++) {
		if (i > pos)
			str[i-1] = str[i];
	}
}

void xstrins(char *str, int pos, char c, int size)
{
	size_t i;
	for (i=size-1; i > pos; i--) {
		str[i] = str[i-1];
	}

	str[pos] = c;
	str[size-1] = '\0';
}

