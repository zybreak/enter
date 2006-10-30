#include <stdlib.h>

#include "greeter_theme.h"
#include "greeter_image.h"
#include "utils.h"

#define BUF_LEN 512

static item_t* load_label(display_t *display, cfg_t *conf, const char *name)
{
	char buf[BUF_LEN];
	item_t *item = xmalloc(sizeof(*item));
	item->type = LABEL;
	
	snprintf(buf,BUF_LEN-1,"%s.%s",name,"x");
	item->label.x = atoi(conf_get(conf,buf));

	snprintf(buf,BUF_LEN-1,"%s.%s",name,"y");
	item->label.y = atoi(conf_get(conf,buf));
	
	snprintf(buf,BUF_LEN-1,"%s.%s",name,"caption");
	item->label.caption = conf_get(conf,buf);

	snprintf(buf,BUF_LEN-1,"%s.%s",name,"font");
	item->label.font = XftFontOpenName(display->dpy,display->screen,conf_get(conf,buf));
	if (!item->label.font) {
		fprintf(stderr,"could not open font \"%s\"\n",conf_get(conf,buf));
		return NULL;
	}
	
	snprintf(buf,BUF_LEN-1,"%s.%s",name,"color");
	item->label.color = xmalloc(sizeof(*(item->label.color)));
	XftColorAllocName(display->dpy,display->visual,display->colormap,conf_get(conf,buf),item->label.color);

	return item;
}

theme_t* theme_new(display_t *display, cfg_t *conf)
{
	char buf[BUF_LEN];
	theme_t *theme = xmalloc(sizeof(*theme));

	theme->display = display;

	theme->title = load_label(display,conf,"title");
	if (!theme->title) {
		free(theme);
		return NULL;
	}
	
	theme->username = load_label(display,conf,"username");
	if (!theme->username) {
		free(theme);
		return NULL;
	}

	snprintf(buf,BUF_LEN-1,"%s/%s",conf_get(conf,"theme_path"),conf_get(conf,"background.image"));
	theme->background = image_load(display,buf);
	if (!theme->background) {
		fprintf(stderr,"could not load image \"%s\"\n",buf);
		/* TODO: free items.  */
		free(theme);
		return NULL;
	}

	return theme;
}

void theme_delete(theme_t *theme)
{
	display_t *display = theme->display;

	/* TODO: clean all items.  */
	free(theme);
}

