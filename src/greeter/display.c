#include <stdlib.h>

#include "display.h"
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
	
	display->gc = XCreateGC(display->dpy,display->root,0,0);
	display->gcm = GCForeground|GCBackground|GCGraphicsExposures;
	
	display->visual = DefaultVisual(display->dpy, display->screen);
	display->colormap = DefaultColormap(display->dpy, display->screen);
	
	return display;
}

