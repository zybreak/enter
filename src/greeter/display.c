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
	
	display->gc = XCreateGC(display->dpy,display->root,0,0);
	display->gcm = GCForeground|GCBackground|GCGraphicsExposures;
	
	display->visual = DefaultVisual(display->dpy, display->screen);
	display->colormap = DefaultColormap(display->dpy, display->screen);
	
	return display;
}

void display_blank(display_t *display)
{
	XSetForeground(display->dpy,display->gc,BlackPixel(display->dpy,display->screen));
	
	XFillRectangle(display->dpy, display->root, display->gc, 0, 0,
		XWidthOfScreen(ScreenOfDisplay(display->dpy, display->screen)),
		XHeightOfScreen(ScreenOfDisplay(display->dpy, display->screen)));
	XFlush(display->dpy);
}
