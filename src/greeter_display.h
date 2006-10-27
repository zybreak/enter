#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <X11/Xlib.h>

typedef struct display_t display_t;

#include "greeter_theme.h"
#include "cfg.h"

struct display_t {
	Window root;
	GC gc;
	int screen;
	int width;
	int height;
	int depth;
	Display *dpy;
	Colormap colormap;
	Visual* visual;
};

display_t* display_new(cfg_t *conf);
void display_delete(display_t *display);
void display_background(display_t *display, theme_t *theme);

#endif /*DISPLAY_H_*/
