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
	
	/* Draw text  */
	XSetForeground(display->dpy, display->gc,BlackPixel(display->dpy,display->screen));
	
	XftColor color;
	XRenderColor color_name;
	color_name.red = color_name.green = color_name.blue = 0xFF;
	
	XftDraw *font = XftDrawCreate(display->dpy,window->win,display->visual,display->colormap);
	
	XftColorAllocValue(display->dpy,display->visual,display->colormap,&color_name,&color);
		
	XftDrawString8(font,&color,theme->title,theme->title_x,theme->title_y,
		(XftChar8*)theme->title_caption,strlen(theme->title_caption));

	XFlush(display->dpy);
	XftDrawDestroy(font);
}

