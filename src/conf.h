#ifndef CONF_H_
#define CONF_H_

typedef struct map_t {
	char *key;
	char *value;
	struct map_t *next;
} map_t;

typedef struct conf_t {
	map_t *map;
} conf_t;

/**
 * Returns a new conf_t object.
 */
conf_t* conf_new(void);
/**
 * Frees conf, and any data contained in conf.
 */
void conf_delete(conf_t *conf);
/**
 * Parses the config file pointed to by config_file
 * and stores the data in conf.
 * Returns FALSE if any error was encountered.
 */
int conf_parse(conf_t *conf, const char *config_file);
/**
 * Returns the key value, or an empty string if it doesnt
 * have a value.
 */
char* conf_get(conf_t *conf, const char *key);
/**
 * Assigns key with the value in ´value´, or overwrites
 * a previous one.
 */
void conf_set(conf_t *conf, const char *key, const char *value);

#endif /*CONF_H_*/
