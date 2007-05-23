#include <X11/keysym.h>
#include <stdlib.h>

#include "enter.h"
#include "gui.h"

#include "login.h"

static void gui_draw(gui_t *gui)
{
	display_t *display = gui->display;
	list_t *it = gui->widgets;

	/* Only clear if double buffering is not present,
	 * since DBE clears the window for us.  */
	if (!display_has_doublebuffer(display)) {
		XClearWindow(display->dpy, gui->win);
	}

	/* Draw the widgets.  */
	if (gui->focus) {
		gui_widget_draw_focus(gui->focus, gui);
	}

	while (it = list_next(it)) {
		if (list_data(it) != gui->focus) {
			gui_widget_draw(list_data(it), gui);
		}
	}

	if (display_has_doublebuffer(display)) {
		XdbeSwapBuffers(display->dpy, &gui->swap_info, 1);
	} else {
		XFlush(display->dpy);
	}
}

static void gui_mappingnotify(gui_t *gui, XEvent *event)
{
	XRefreshKeyboardMapping(&event->xmapping);
}

static void gui_keypress(gui_t *gui, XEvent *event)
{
}

gui_t* gui_new(display_t *display)
{
	gui_t *gui = (gui_t*)xmalloc(sizeof(*gui));
	memset(gui,'\0',sizeof(*gui));

	/* Set default options.  */
	gui->x = 0;
	gui->y = 0;
	gui->width = display->width;
	gui->height = display->height;
	gui->display = display;
	gui->widgets = list_new();
	gui->focus = NULL;

	/*
	gui->swap_info = {
		.swap_window = gui->win,
		.swap_action = XdbeBackground
	};
	*/

	unsigned long color = BlackPixel(display->dpy, display->screen);

	/* Create the GUI window.  */
	gui->win = XCreateSimpleWindow(display->dpy, display->root,
		gui->x, gui->y, gui->width, gui->height,
		0, color, color);

	if (display_has_doublebuffer(display)) {
		gui->back_buffer = XdbeAllocateBackBufferName(display->dpy,
			gui->swap_info.swap_window, gui->swap_info.swap_action);
		gui->drawable = gui->back_buffer;
	} else {
		gui->drawable = gui->win;
	}

	/* Create a draw surface.  */
	gui->draw = XftDrawCreate(display->dpy, gui->drawable, display->visual,
					display->colormap);

	return gui;
}

void gui_delete(gui_t *gui)
{
	display_t *display = gui->display;

	if (display_has_doublebuffer(display))
		XdbeDeallocateBackBufferName(display->dpy, gui->back_buffer);
	
	XftDrawDestroy(gui->draw);
	XDestroyWindow(display->dpy, gui->win);

	list_delete(gui->widgets);	
	free(gui);
}

void gui_show(gui_t *gui)
{
	display_t *display = gui->display;

	XSelectInput(display->dpy, gui->win, ExposureMask | KeyPressMask);

	XMapWindow(display->dpy, gui->win);
	XMoveWindow(display->dpy, gui->win, gui->x, gui->y);

	XGrabKeyboard(display->dpy, gui->win, False, GrabModeAsync,
			GrabModeAsync, CurrentTime);

	gui_draw(gui);

	XFlush(display->dpy);
}

void gui_hide(gui_t *gui)
{
	display_t *display = gui->display;

	XUnmapWindow(display->dpy, gui->win);
	XUngrabKeyboard(display->dpy, CurrentTime);

	XFlush(display->dpy);
}

void gui_next_event(gui_t *gui)
{
	XEvent event;
	
	XNextEvent(gui->display->dpy, &event);

	switch(event.type) {
		case Expose:
			gui_draw(gui);
			break;
		case KeyPress:
			gui_keypress(gui, &event);
			break;
		case MappingNotify:
			gui_mappingnotify(gui, &event);
			break;
	}
}

