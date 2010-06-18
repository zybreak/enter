#include <stdlib.h>

#include "enter.h"

#include "display.h"
#include "utils.h"

display_t* display_new(const char *display_name)
{
	int i;
	int n_screens;
	display_t *display = g_new(display_t,1);

	display->dpy = xcb_connect(display_name,&n_screens);
	if (xcb_connection_has_error(display->dpy)) {
		g_free(display);
		return NULL;
	}

	const xcb_setup_t *setup = xcb_get_setup(display->dpy);
	xcb_screen_iterator_t iter = xcb_setup_roots_iterator(setup);

	for (i = 0; i < n_screens; ++i) {
		xcb_screen_next(&iter);
	}

	display->screen = iter.data;

	return display;
}

void display_delete(display_t *display)
{
	xcb_disconnect(display->dpy);
	g_free(display);
}

void display_kill_clients(display_t *display, xcb_window_t window)
{
	return;
}

