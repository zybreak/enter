#ifndef WINDOW_H_
#define WINDOW_H_

#include <X11/Xlib.h>
#include <X11/Xmu/WinUtil.h>

#include "display.h"
#include "cfg.h"

typedef struct window_t {
	Window win;
	int x, y;
	int width, height;
	display_t *display;
	Pixmap background;
	cfg_t *conf;
} window_t;

window_t* window_init(display_t *display, cfg_t *conf);
void window_show(window_t *window);

#endif /*WINDOW_H_*/
