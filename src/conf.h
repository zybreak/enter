#ifndef CONF_H_
#define CONF_H_

/**
 * The conf_t struct is just a linked list.
 */
typedef GKeyFile conf_t;

/**
 * Creates a new conf_t object. 
 * @return A new conf_t object.
 */
conf_t* conf_new(void);

/**
 * Deletes a conf_t object, but not the stored data.
 * @param conf the object to free.
 */
void conf_delete(conf_t *conf);

/**
 * Parses a configuration into a conf_t object.
 * @param conf The object to store the data in.
 * @param config_file Which configuration file to use.
 * @return TRUE if successful, otherwise FALSE.
 */
int conf_parse(conf_t *conf, const gchar *config_file);

/**
 * Return the value associated with a specified key.
 * @param conf Which conf_t object to use.
 * @param key Which key to use.
 * @return The value associated with key, or EMPTY_DATA.
 */
char* conf_get(conf_t *conf, const gchar *key);

/**
 * Assign key a specified value.
 * @param conf Which conf_t object to use.
 * @param key Which key to assign a value to.
 * @param value The value to assign.
 */
void conf_set(conf_t *conf, const gchar *key, const gchar *value);

/**
 * Merge two conf_t objects.
 * @param to Which object to store the data in.
 * @param from The object to read the data from.
 */
void conf_merge(conf_t *to, conf_t *from);

#endif /*CONF_H_*/
