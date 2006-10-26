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

	/* TODO: wrong dimensions.  */
	window->win = XCreateSimpleWindow(display->dpy, display->root,
		window->x, window->y, window->width, window->height,
		0, color, color);

	return window;
}

void window_show(window_t *window)
{
	display_t *display = window->display;
	theme_t *theme = window->theme;

	XSelectInput(display->dpy, window->win, ExposureMask | KeyPressMask);

	/* TODO: Wrong image.  */
	XSetWindowBackgroundPixmap(display->dpy, window->win, theme->background);

	XMapWindow(display->dpy, window->win);
	XMoveWindow(display->dpy, window->win, window->x, window->y);

	XGrabKeyboard(display->dpy, window->win, False, GrabModeAsync, GrabModeAsync, CurrentTime);

	XFlush(display->dpy);
}

void window_events(window_t *window, XEvent *event)
{
	display_t *display = window->display;
	theme_t *theme = window->theme;

	switch(event->type) {
	case Expose: {
		XftDraw *d = XftDrawCreate(display->dpy,window->win,display->visual,display->colormap);
		XftDrawString8(d,theme->title_color,theme->title,
				theme->title_x,theme->title_y,
				(XftChar8*)theme->title_caption,strlen(theme->title_caption));
		XftDrawDestroy(d);
		
		XFlush(display->dpy);
		break;
	}
	case KeyPress:
		printf("key press\n");
		break;
	}
}
