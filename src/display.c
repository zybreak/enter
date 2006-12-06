#include <stdlib.h>

#include "display.h"
#include "utils.h"

display_t* display_new(cfg_t *conf)
{
	display_t *display = (display_t*)xmalloc(sizeof(display_t));

	display->dpy = XOpenDisplay(conf_get(conf, "display"));
	if (!display->dpy) {
		free(display);
		return NULL;
	}
	
	display->screen = DefaultScreen(display->dpy);
	display->root = RootWindow(display->dpy,display->screen);
	display->depth = DefaultDepth(display->dpy,display->screen);

	display->width = XWidthOfScreen(
			ScreenOfDisplay(display->dpy, display->screen));
	display->height = XHeightOfScreen(
			ScreenOfDisplay(display->dpy, display->screen));
	XGCValues values;
	values.foreground = WhitePixel(display->dpy,display->screen);
	values.background = BlackPixel(display->dpy,display->screen);
	values.graphics_exposures = False;

	display->gc = XCreateGC(display->dpy,display->root,
		GCForeground|GCBackground|GCGraphicsExposures,
		&values); 
	
	display->visual = DefaultVisual(display->dpy, display->screen);
	display->colormap = DefaultColormap(display->dpy, display->screen);
	
	return display;
}

void display_delete(display_t *display)
{
	XFreeGC(display->dpy,display->gc);
	XCloseDisplay(display->dpy);
	free(display);
}
