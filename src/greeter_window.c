#include <stdlib.h>

#include "enter.h"
#include "greeter_window.h"
#include "greeter_image.h"
#include "utils.h"

#define DRAW_STRING(drawable,item) \
	XftDrawString8(drawable,item.color,item.font, \
		item.x,item.y, \
		(XftChar8*)item.caption,strlen(item.caption));

static void redraw(window_t *window)
{
	display_t *display = window->display;
	theme_t *theme = window->theme;
	
	XGlyphInfo extents;
	XftDraw *d = XftDrawCreate(display->dpy,window->win,display->visual,display->colormap);
	
	XClearWindow(display->dpy,window->win);

	DRAW_STRING(d,theme->title);
	DRAW_STRING(d,theme->username);
	XftTextExtents8(display->dpy,theme->username.font,
		(XftChar8*)theme->username.caption,
		strlen(theme->username.caption), &extents);

	int rect_width = /*rect_x + */extents.width;
	int rect_height = /*rect_y + */extents.height;
	int rect_x = rect_width*2 + theme->username.x - extents.x;
	int rect_y = theme->username.y - extents.y;

	XftDrawRect(d,theme->username.color,
			rect_x,rect_y,rect_width,rect_height);

	XftDrawDestroy(d);

	XFlush(display->dpy);
}

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

	XSetWindowBackgroundPixmap(display->dpy, window->win, theme->background);

	XMapWindow(display->dpy, window->win);
	XMoveWindow(display->dpy, window->win, window->x, window->y);

	XGrabKeyboard(display->dpy, window->win, False, GrabModeAsync, GrabModeAsync, CurrentTime);

	XFlush(display->dpy);
}

void window_events(window_t *window, XEvent *event)
{
	switch(event->type) {
	case Expose:
		redraw(window);
		break;
	case KeyPress:
		printf("key press\n");
		break;
	}
}

