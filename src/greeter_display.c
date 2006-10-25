#include <stdlib.h>

#include "greeter_display.h"
#include "utils.h"

static unsigned long get_color(Display *dpy, Window root, const char* colorname)
{
    XColor color;
    XWindowAttributes attributes;

    XGetWindowAttributes(dpy, root, &attributes);
    color.pixel = 0;
    
    XParseColor(dpy, attributes.colormap, colorname, &color);
    XAllocColor(dpy, attributes.colormap, &color);

    return color.pixel;
}

display_t* display_init()
{
	XGCValues values;
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
	
	values.foreground = get_color(display->dpy, display->root, "black");
	values.background = get_color(display->dpy, display->root, "white");
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
	/* TODO: Delete GC. */
	XCloseDisplay(display->dpy);
}
