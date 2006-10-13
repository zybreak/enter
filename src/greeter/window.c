#include <stdlib.h>

#include "window.h"

window_t* window_init(display_t *display, cfg_t *conf)
{
	XColor color;
    XWindowAttributes attributes;
    
    window_t *window  = malloc(sizeof *window);
    if (!window)
    	return NULL;
    
    window->x = 0;
	window->y = 0;
	window->display = display;
	window->conf = conf;

	color.pixel = 0;

    XGetWindowAttributes(display->dpy, display->root, &attributes);

    if(!XParseColor(display->dpy, attributes.colormap, "white", &color))
        return NULL;
    
    if(!XAllocColor(display->dpy, attributes.colormap, &color))
        return NULL;
    
	window->win = XCreateSimpleWindow(display->dpy, display->root, window->x, window->y,
                              //image->Width(),
                              //image->Height(),
                              10,
                              10,
                              0, color.pixel, color.pixel);
	return window;
}

void window_show(window_t *window)
{
	display_t *display = window->display;
	
	XSelectInput(display->dpy, window->win, ExposureMask | KeyPressMask);

	//XSetWindowBackgroundPixmap(display->dpy, window->win, window->pixmap);

	XMapWindow(display->dpy, window->win);
    XMoveWindow(display->dpy, window->win, window->x, window->y);

    XGrabKeyboard(display->dpy, window->win, False, GrabModeAsync, GrabModeAsync, CurrentTime);

    XFlush(display->dpy);
}
