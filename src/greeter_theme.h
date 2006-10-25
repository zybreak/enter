#ifndef __THEME_H__
#define __THEME_H__

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include "greeter_display.h"
#include "cfg.h"

typedef struct theme_t {
	display_t *display;

	XftFont *title;
	XftColor *title_color;
	int title_x, title_y;
	char *title_caption;
	
	Pixmap background;
} theme_t;

theme_t* theme_new(display_t *display, cfg_t *conf);
void theme_delete(theme_t *theme);

#endif /* __THEME_H__  */
