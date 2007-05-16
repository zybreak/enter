#ifndef CONF_H_
#define CONF_H_

#include "list.h"

/**
 * The conf_t struct is just a linked list
 */
typedef list_t conf_t;

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
/**
 * Assign conf `to' all keys from conf `from'.
 */
void conf_merge(conf_t *to, conf_t *from);

#endif /*CONF_H_*/
