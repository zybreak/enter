#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "enter.h"
#include "conf.h"
#include "utils.h"

#define BUFFER_SIZE 200
#define EMPTY_DATA ""

typedef struct pair_t {
	char *key;
	char *value;
} pair_t;

static void get_opt_arg(char *str, char **opt, char **arg)
{
	char *p = index(str,'=');
	
	if (!p)
		p = str+strlen(str)-1;
	else
		*p='\0';

	*opt = xstrtrim(str);
	*arg = xstrtrim(++p);
}

static pair_t* new_pair(char *key, char *value)
{
	pair_t *pair = xmalloc(sizeof(*pair));
	
	pair->key = key;
	pair->value = value;
	
	return pair;
}

conf_t* conf_new(void)
{
	conf_t *conf = xmalloc(sizeof(*conf));

	return conf;
}

void conf_delete(conf_t *conf)
{
	list_delete(conf);
}

int conf_parse(conf_t *conf, const char *config_file)
{
	char buffer[BUFFER_SIZE];
	char *opt,*arg;
	
	FILE *fp = fopen(config_file,"r");
	
	if (!fp) {
		return FALSE;
	}
	
	memset(buffer,'\0',BUFFER_SIZE);

	while (fgets(buffer,BUFFER_SIZE-1,fp)) {
		get_opt_arg(buffer,&opt,&arg);

		/* Filter out comments and empty lines.  */
		if ((*opt == '#') || !(*opt))
			continue;

		conf_set(conf,strdup(opt),strdup(arg));
	}

	fclose(fp);
	
	return TRUE;
}

char* conf_get(conf_t *conf, char *key)
{
	/* Skip head.  */
	conf = list_next(conf);

	while (conf) {
		if (!strcmp(((pair_t*)list_data(conf))->key, key))
			return ((pair_t*)list_data(conf))->value;
		conf = list_next(conf);
	}
	
	return EMPTY_DATA;
}

void conf_set(conf_t *conf, char *key, char *value)
{
	/* Skip the first node, since its never used.  */
	while (list_next(conf)) {
		conf = list_next(conf);

		if (!strcmp(((pair_t*)list_data(conf))->key, key)) {
			((pair_t*)list_data(conf))->value = value;
			return;
		}
	}

	list_add(conf, new_pair(key, value));
}

void conf_merge(conf_t *to, conf_t *from)
{
	/* Skip head.  */
	while (from = list_next(from)) {
		conf_set(to, ((pair_t*)list_data(from))->key, ((pair_t*)list_data(from))->value);
	}
}

