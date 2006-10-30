#include <stdlib.h>

#include "enter.h"
#include "greeter_window.h"
#include "greeter_image.h"
#include "utils.h"

#define DRAW_STRING(drawable,label) \
	XftDrawString8(drawable,label.color,label.font, \
		label.x,label.y, \
		(XftChar8*)label.caption,strlen(label.caption));

static void redraw(window_t *window)
{
	display_t *display = window->display;
	theme_t *theme = window->theme;
	label_t *label;
	
	XftDraw *d = XftDrawCreate(display->dpy,window->win,display->visual,display->colormap);
	
	XClearWindow(display->dpy,window->win);

	label = &theme->title->label;
	XftDrawString8(d,label->color,label->font, \
		label->x,label->y, \
		(XftChar8*)label->caption,strlen(label->caption));

	label = &theme->username->label;
	XftDrawString8(d,label->color,label->font, \
		label->x,label->y, \
		(XftChar8*)label->caption,strlen(label->caption));

	label = &theme->username->label;
	XftDrawString8(d,label->color,label->font, \
		400,label->y, \
		(XftChar8*)window->input,strlen(window->input));

	XftDrawDestroy(d);

	XFlush(display->dpy);
}

static void keypress(window_t *window, XEvent *event)
{
	char ch;
	KeySym keysym;
	XComposeStatus cstatus;

	XLookupString(&event->xkey,&ch,1,&keysym,&cstatus);

	if (keysym == XK_BackSpace) {
		if (window->input_pos > 0)
			window->input[--window->input_pos] = '\0';
	} else if (window->input_pos<INPUT_LEN-1) {
		window->input[window->input_pos++] = ch;
	} else
		return;

	redraw(window);
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

	memset(window->input,'\0',INPUT_LEN);
	window->input_pos = 0;

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
		keypress(window,event);
		break;
	}
}

