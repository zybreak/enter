#ifndef CONF_H_
#define CONF_H_

typedef struct map_t {
	char *key;
	char *value;
	struct map_t *next;
} map_t;

typedef struct cfg_t {
	map_t *map;
} cfg_t;

cfg_t* conf_new(void);
void conf_delete(cfg_t *conf);
int conf_parse(cfg_t *conf, const char *config_file);
char* conf_get(cfg_t *conf, const char *key);
void conf_set(cfg_t *conf, const char *key, const char *value);

#endif /*CONF_H_*/
