#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "enter.h"
#include "conf.h"
#include "utils.h"

#define BUFFER_SIZE 200
#define EMPTY_DATA ""

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

static conf_t* new_pair(char *key, char *value)
{
	conf_t *item = conf_new();
	
	item->key = key;
	item->value = value;
	
	return item;
}

conf_t* conf_new(void)
{
	conf_t *conf = xmalloc(sizeof(*conf));
	memset(conf, 0, sizeof(*conf));

	return conf;
}

void conf_delete(conf_t *conf)
{
	while (conf) {
		conf_t *next = conf->next;
		
		free(conf);

		conf = next;
	}
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

		conf_set(conf,opt,arg);
	}

	fclose(fp);
	
	return TRUE;
}

char* conf_get(conf_t *conf, char *key)
{
	while (conf) {
		if (!strcmp(conf->key,key))
			return conf->value;
		conf = conf->next;
	}
	
	return EMPTY_DATA;
}

void conf_set(conf_t *conf, char *key, char *value)
{
	conf_t *c = conf->next;
	
	if (!conf->next) {
		conf->next = new_pair(key, value);
		return;
	}
	
	while (1) {
		if (!strcmp(c->key, key)) {
			c->value = value;
			return;
		} else if (!c->next)
			break;

		c = c->next;
	}

	c->next = new_pair(key, value);
}

