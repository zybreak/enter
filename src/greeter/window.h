#ifndef WINDOW_H_
#define WINDOW_H_

#include <X11/Xlib.h>

#include "display.h"
#include "cfg.h"

typedef struct {
	Window win;
	int x, y;
	display_t *display;
	cfg_t *conf;
} window_t;

window_t* window_init(display_t *display, cfg_t *conf);
void window_show(window_t *window);

#endif /*WINDOW_H_*/
