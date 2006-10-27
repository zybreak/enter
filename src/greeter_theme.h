#ifndef __THEME_H__
#define __THEME_H__

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

typedef struct theme_t theme_t;
typedef struct item_t item_t;

#include "greeter_display.h"
#include "cfg.h"

struct theme_t {
	display_t *display;

	struct item_t {
		XftFont *font;
		XftColor *color;
		int x, y;
		char *caption;
	} title, username, password;
	
	Pixmap background;
};

theme_t* theme_new(display_t *display, cfg_t *conf);
void theme_delete(theme_t *theme);

#endif /* __THEME_H__  */
