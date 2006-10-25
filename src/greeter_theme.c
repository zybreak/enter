#include <stdlib.h>

#include "greeter_theme.h"
#include "greeter_image.h"
#include "utils.h"

theme_t* theme_new(display_t *display, cfg_t *conf)
{
	theme_t *theme = xmalloc(sizeof(*theme));

	theme->display = display;

	theme->title_x = atoi(conf_get(conf,"title.x"));
	theme->title_y = atoi(conf_get(conf,"title.y"));
	theme->title_caption = conf_get(conf,"title.caption");

	theme->title = XftFontOpenName(display->dpy,display->screen,
			conf_get(conf,"title.font"));
	if (!theme->title) {
		free(theme);
		return NULL;
	}
	
	theme->title_color = xmalloc(sizeof(*(theme->title_color)));
	XftColorAllocName(display->dpy,display->visual,display->colormap,conf_get(conf,"title.color"),theme->title_color);

	theme->background = image_load(display,
			conf_get(conf,"background.image"));
	if (!theme->background) {
		/* TODO: free font.  */
		free(theme);
		return NULL;
	}

	return theme;
}

void theme_delete(theme_t *theme)
{
	display_t *display = theme->display;

	XftColorFree(display->dpy,display->visual,display->colormap,theme->title_color);
	free(theme);
}

