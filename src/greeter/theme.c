#include <stdlib.h>

#include "theme.h"
#include "cfg.h"

XImage* load_img(display_t *display, const char* image)
{
	
}

theme_t* theme_init(display_t *display, const char *theme_path)
{
	theme_t *theme = (theme_t*)xmalloc(sizeof *theme);
	cfg_t *conf = conf_new();
	if (!conf_parse(conf,theme_path)) {
		free(theme);
		free(conf);
		return NULL;
	}
	
	/* TODO: load images.  */

}
