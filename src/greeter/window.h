#ifndef WINDOW_H_
#define WINDOW_H_

#include <X11/Xlib.h>

#include "display.h"
#include "theme.h"

typedef struct window_t {
	Window win;
	int x, y;
	int width, height;
	display_t *display;
	theme_t *theme;
	Pixmap background;
} window_t;

window_t* window_new(display_t *display, theme_t *theme);
void window_show(window_t *window);

#endif /*WINDOW_H_*/
