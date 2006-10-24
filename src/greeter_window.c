#include <stdlib.h>

#include "enter.h"
#include "greeter_window.h"
#include "greeter_image.h"
#include "utils.h"

window_t* window_new(display_t *display, theme_t *theme)
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
	window->theme = theme;

	color = BlackPixel(display->dpy,display->screen);

	window->win = display->root;
		/*
		XCreateSimpleWindow(display->dpy, display->root,
		window->x, window->y, window->width, window->height,
		0, color, color);
		*/
	
	window->background = XCreatePixmap(display->dpy,window->win,
			theme->background->width,theme->background->height,
			display->depth);

	image_to_pixmap(window,theme->background,window->background);

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

