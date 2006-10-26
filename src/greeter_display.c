#include <stdlib.h>

#include "greeter_display.h"
#include "utils.h"

display_t* display_init()
{
	display_t *display = (display_t*)xmalloc(sizeof(display_t));
	
	display->dpy = XOpenDisplay(NULL);
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
}

void display_background(display_t *display, theme_t *theme)
{
	XSetWindowBackgroundPixmap(display->dpy, display->root, theme->background);
}

