#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <X11/Xlib.h>

typedef struct display_t {
	Window root;
	GC gc;
	int screen;
	int width;
	int height;
	int depth;
	Display *dpy;
	unsigned long gcm;
	Colormap colormap;
	Visual* visual;
} display_t;

display_t* display_init();

#endif /*DISPLAY_H_*/
