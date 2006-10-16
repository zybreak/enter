#include <stdlib.h>

#include "enter.h"
#include "window.h"
#include "utils.h"
#include "image.h"

window_t* window_init(display_t *display, cfg_t *conf)
{
	unsigned long color;

	window_t *window  = xmalloc(sizeof *window);
	if (!window)
		return NULL;

	window->x = 0;
	window->y = 0;
	window->width = display->width;
	window->height = display->height;
	window->display = display;
	window->conf = conf;

	color = BlackPixel(display->dpy,display->screen);

	window->win = XCreateSimpleWindow(display->dpy, display->root,
		window->x, window->y, window->width, window->height,
		0, color, color);
	
	window->background = XCreatePixmap(display->dpy,window->win,
			window->width,window->height,display->depth);

	if (!load_image(display,window->background,"image.png")) {
		free(window);
		return NULL;
	}

	return window;
}

void window_show(window_t *window)
{
	display_t *display = window->display;
	
	XSelectInput(display->dpy, window->win, ExposureMask | KeyPressMask);

	XSetWindowBackgroundPixmap(display->dpy, window->win, window->background);

	XMapWindow(display->dpy, window->win);
	XMoveWindow(display->dpy, window->win, window->x, window->y);

	XGrabKeyboard(display->dpy, window->win, False, GrabModeAsync, GrabModeAsync, CurrentTime);

	XFlush(display->dpy);
}
