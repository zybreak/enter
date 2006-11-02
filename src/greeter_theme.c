#include <stdlib.h>

#include "greeter_theme.h"
#include "greeter_image.h"
#include "utils.h"

#define BUF_LEN 512

static label_t* load_label(display_t *display, cfg_t *conf, const char *name)
{
	char buf[BUF_LEN];
	label_t *label = xmalloc(sizeof(*label));
	
	snprintf(buf,BUF_LEN-1,"%s.%s",name,"x");
	label->x = atoi(conf_get(conf,buf));

	snprintf(buf,BUF_LEN-1,"%s.%s",name,"y");
	label->y = atoi(conf_get(conf,buf));
	
	snprintf(buf,BUF_LEN-1,"%s.%s",name,"caption");
	label->caption = conf_get(conf,buf);

	snprintf(buf,BUF_LEN-1,"%s.%s",name,"font");
	label->font = XftFontOpenName(display->dpy,display->screen,conf_get(conf,buf));
	if (!label->font) {
		fprintf(stderr,"could not open font \"%s\"\n",conf_get(conf,buf));
		return NULL;
	}
	
	snprintf(buf,BUF_LEN-1,"%s.%s",name,"color");
	label->color = xmalloc(sizeof(*(label->color)));
	XftColorAllocName(display->dpy,display->visual,display->colormap,conf_get(conf,buf),label->color);

	return label;
}

static void free_label(display_t *display, label_t *label)
{
	XftColorFree(display->dpy,display->visual,
			display->colormap,label->color);
	free(label);
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
		free_label(display,theme->title);
		free(theme);
		return NULL;
	}

	snprintf(buf,BUF_LEN-1,"%s/%s",conf_get(conf,"theme_path"),conf_get(conf,"background.image"));
	theme->background = image_load(display,buf);
	if (!theme->background) {
		fprintf(stderr,"could not load image \"%s\"\n",buf);

		free_label(display,theme->title);
		free_label(display,theme->username);
		free(theme);
		return NULL;
	}

	return theme;
}

void theme_delete(theme_t *theme)
{
	display_t *display = theme->display;

	free_label(display,theme->title);
	free_label(display,theme->username);
	
	XFreePixmap(display->dpy,theme->background);
	
	free(theme);
}

