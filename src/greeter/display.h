#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <X11/Xlib.h>

typedef struct {
	Window root;
	GC gc;
	int screen;
	Display *dpy;
	unsigned long gcm;
	Colormap colormap;
	Visual* visual;
} display_t;

display_t* display_init();
void display_blank(display_t *display);

#endif /*DISPLAY_H_*/
