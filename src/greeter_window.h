#ifndef WINDOW_H_
#define WINDOW_H_

#include <X11/Xlib.h>

#include "greeter_display.h"
#include "greeter_theme.h"

#define INPUT_LEN 20

typedef struct window_t {
	Window win;
	int x, y;
	int width, height;
	display_t *display;
	theme_t *theme;
	/* TODO: This should be cleaned up, only here for testing pourposes. */
	char input[INPUT_LEN];
	int input_pos;
} window_t;

window_t* window_new(display_t *display, theme_t *theme);
void window_show(window_t *window);
void window_events(window_t *window, XEvent *event);

#endif /*WINDOW_H_*/
