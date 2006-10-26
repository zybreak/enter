#include <stdlib.h>

#include "greeter_theme.h"
#include "greeter_image.h"
#include "utils.h"

#define BUF_LEN 512

theme_t* theme_new(display_t *display, cfg_t *conf)
{
	char buf[BUF_LEN];
	theme_t *theme = xmalloc(sizeof(*theme));

	theme->display = display;

	theme->title_x = atoi(conf_get(conf,"title.x"));
	theme->title_y = atoi(conf_get(conf,"title.y"));
	theme->title_caption = conf_get(conf,"title.caption");

	theme->title = XftFontOpenName(display->dpy,display->screen,
			conf_get(conf,"title.font"));
	if (!theme->title) {
		fprintf(stderr,"could not open font \"%s\"\n",conf_get(conf,"title.font"));
		free(theme);
		return NULL;
	}
	
	theme->title_color = xmalloc(sizeof(*(theme->title_color)));
	XftColorAllocName(display->dpy,display->visual,display->colormap,conf_get(conf,"title.color"),theme->title_color);

	snprintf(buf,BUF_LEN-1,"%s/%s",conf_get(conf,"theme_path"),conf_get(conf,"background.image"));
	theme->background = image_load(display,buf);
	if (!theme->background) {
		fprintf(stderr,"could not load image \"%s\"\n",buf);
		/* TODO: free theme->title.  */
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

