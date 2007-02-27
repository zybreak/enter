#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <X11/Xlib.h>

#include "conf.h"

typedef struct display_t {
	Window root;
	GC gc;
	int screen;
	int width;
	int height;
	int depth;
	Display *dpy;
	Colormap colormap;
	Visual* visual;
} display_t;

/**
 * Returns a new display_t object,
 * uses conf to get the DISPLAY string.
 */
display_t* display_new(conf_t *conf);

/**
 * Frees the memory occupied by display.

*/
void display_delete(display_t *display);

/**
 * Kills all clients, except `window', running on the display.
 */
void display_kill_clients(display_t *display, Window window);

#endif /*DISPLAY_H_*/
