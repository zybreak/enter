#ifndef CONF_H_
#define CONF_H_

/**
 * The first node in the list is just a pointer to the
 * head and should not contain any other data.
 */
typedef struct conf_t {
	char *key;
	char *value;
	struct conf_t *next;
} conf_t;

/**
 * Returns a new conf_t object.
 */
conf_t* conf_new(void);
/**
 * Free's conf, but not any data.
 */
void conf_delete(conf_t *conf);
/**
 * Parses the config file pointed to by config_file
 * and stores the data in conf.
 * Returns FALSE if any error was encountered.
 */
int conf_parse(conf_t *conf, const char *config_file);
/**
 * Returns the key value, or an empty string if it doesn't
 * have a value.
 */
char* conf_get(conf_t *conf, const char *key);
/**
 * Assigns key with the value in ´value´, or overwrites
 * a previous one.
 */
void conf_set(conf_t *conf, const char *key, const char *value);

#endif /*CONF_H_*/
