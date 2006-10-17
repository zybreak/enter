#include <stdlib.h>

#include "theme.h"
#include "utils.h"
#include "image.h"

theme_t* theme_new(display_t *display, cfg_t *conf)
{
	theme_t *theme = xmalloc(sizeof(*theme));

	theme->title_x = atoi(conf_get(conf,"title.x"));
	theme->title_y = atoi(conf_get(conf,"title.y"));
	theme->title_caption = conf_get(conf,"title.caption");

	theme->title = XftFontOpenName(display->dpy,display->screen,
			conf_get(conf,"title.font"));
	if (!theme->title)
		return NULL;

	theme->background = image_load(display,
			conf_get(conf,"background.image"));
	if (!theme->background)
		return NULL;

	return theme;
}

