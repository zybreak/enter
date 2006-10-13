#ifndef THEME_H_
#define THEME_H_

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/Xft/Xft.h>

#include "display.h"

typedef struct {
	XImage *background;
	XImage *panel;
	XftFont* font;
	XftColor color;
} theme_t;

theme_t* theme_init(display_t *display, const char *theme_path);

#endif /*THEME_H_*/
