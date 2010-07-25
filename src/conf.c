#include "enter.h"

#include "conf.h"

#define EMPTY_DATA ""

conf_t* conf_new(void)
{
	conf_t *conf = g_key_file_new();

	return conf;
}

void conf_delete(conf_t *conf)
{
	g_key_file_free(conf);
}

int conf_parse(conf_t *conf, const gchar *config_file)
{
	GError *error = NULL;
	gboolean ret;
	
	ret = g_key_file_load_from_file(conf,config_file,G_KEY_FILE_NONE, &error);

	if (ret == FALSE) {
		if (error) {
			g_error_free(error);
		}
		return FALSE;
	}

	return TRUE;
}

gchar* conf_get(conf_t *conf, const gchar *key)
{
	GError *error = NULL;
	gchar *ret;

	ret = g_key_file_get_string(conf,"enter",key, &error);

	if (ret) {
		return ret;
	}

	if (error) {
		g_error_free(error);
	}

	return EMPTY_DATA;
}

void conf_set(conf_t *conf, const gchar *key, const gchar *value)
{
	g_key_file_set_string(conf,"enter",key,value);
}

void conf_merge(conf_t *to, conf_t *from)
{
	GError *error = NULL;
	gsize length;
	gchar **keys;
	gchar *key;
	int i = 0;

	keys = g_key_file_get_keys(from,"enter",&length,&error);

	if (keys) {
		while (i<length) {
			key = keys[i++];
			gchar *value = conf_get(from,key);
			conf_set(to,key,value);
		}
		g_strfreev(keys);
	}
	
	if (error) {
		g_error_free(error);
	}
}

