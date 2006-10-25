#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "enter.h"
#include "cfg.h"
#include "utils.h"

#define BUFFER_SIZE 200

static void get_opt_arg(char *str, char **opt, char **arg)
{
	char *p = index(str,'=');
	
	if (!p)
		p = str+strlen(str)-1;
	else
		*p='\0';

	*opt = strtrim(str);
	*arg = strtrim(++p);
}

static map_t* map_new(const char *key, const char *value)
{
	map_t *map = xmalloc(sizeof(*map));
	map->key = strdup(key);
	map->value = strdup(value);
	map->next = NULL;
	
	return map;
}

cfg_t* conf_new(void)
{
	cfg_t *conf = xmalloc(sizeof(*conf));
	conf->map = NULL;
	
	return conf;
}

int conf_parse(cfg_t *conf, const char *config_file)
{
	char buffer[BUFFER_SIZE];
	char *opt,*arg;
	
	FILE *fp = fopen(config_file,"r");
	
	if (!fp) {
		perror("Could not read config");
		return FALSE;
	}
	
	memset(buffer,'\0',BUFFER_SIZE);

	while (fgets(buffer,BUFFER_SIZE-1,fp)) {
		get_opt_arg(buffer,&opt,&arg);

		/* Filter out comments and empty lines.  */
		if ((*opt!='#') && (*opt))
			conf_set(conf,opt,arg);
	}

	fclose(fp);
	
	return TRUE;
}

char* conf_get(cfg_t *conf, const char *key)
{
	map_t *map = conf->map;
	
	while (map) {
		if (!strcmp(map->key,key))
			return map->value;
		map = map->next;
	}
	return "";
}

/* TODO: how should conf_set handle new data?
 * right now it stores duplicates of its indata, and not the
 * indata itself to assure it can free it when needed.
 * Although if the responsibility to free the data is on the user,
 * this will not be needed. Then ofcource overwriting data is not
 * so easy anymore.  */
void conf_set(cfg_t *conf, const char *key, const char *value)
{
	map_t *map = conf->map;

	if (!map) {
		conf->map = map_new(key,value);
		return;
	}
	
	while (1) {
		if (!strcmp(map->key,key)) {
			if (map->value)
				free(map->value);
			map->value = strdup(value);
			return;
		} else if (!map->next)
			break;

		map = map->next;
	}

	map->next = map_new(key,value);
}
