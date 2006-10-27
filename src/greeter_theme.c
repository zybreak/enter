#include <stdlib.h>

#include "enter.h"
#include "greeter_theme.h"
#include "greeter_image.h"
#include "utils.h"

#define BUF_LEN 512

static int load_item(display_t *display, cfg_t *conf, const char *name, item_t *item)
{
	char buf[BUF_LEN];
	snprintf(buf,BUF_LEN-1,"%s.%s",name,"x");
	item->x = atoi(conf_get(conf,buf));
	snprintf(buf,BUF_LEN-1,"%s.%s",name,"y");
	item->y = atoi(conf_get(conf,buf));
	snprintf(buf,BUF_LEN-1,"%s.%s",name,"caption");
	item->caption = conf_get(conf,buf);

	snprintf(buf,BUF_LEN-1,"%s.%s",name,"font");
	item->font = XftFontOpenName(display->dpy,display->screen,conf_get(conf,buf));
	if (!item->font) {
		fprintf(stderr,"could not open font \"%s\"\n",conf_get(conf,buf));
		return FALSE;
	}
	
	snprintf(buf,BUF_LEN-1,"%s.%s",name,"color");
	item->color = xmalloc(sizeof(*(item->color)));
	XftColorAllocName(display->dpy,display->visual,display->colormap,conf_get(conf,buf),item->color);

	return TRUE;

}

theme_t* theme_new(display_t *display, cfg_t *conf)
{
	char buf[BUF_LEN];
	theme_t *theme = xmalloc(sizeof(*theme));

	theme->display = display;

	if (!load_item(display,conf,"title",&theme->title)) {
		free(theme);
		return NULL;
	}
	
	if (!load_item(display,conf,"username",&theme->username)) {
		free(theme);
		return NULL;
	}

	if (!load_item(display,conf,"password",&theme->password)) {
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

	XftColorFree(display->dpy,display->visual,display->colormap,theme->title.color);
	free(theme);
}

