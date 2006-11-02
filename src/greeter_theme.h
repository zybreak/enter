#ifndef __THEME_H__
#define __THEME_H__

#include "enter.h"

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

typedef struct theme_t theme_t;
typedef union item_t item_t;

typedef struct label_t {
	XftFont *font;
	XftColor *color;
	char *caption;
	int x, y;
} label_t;

typedef struct image_t {
	Pixmap image;
	int x, y;
} image_t;

#include "greeter_display.h"
#include "cfg.h"

struct theme_t {
	display_t *display;

	label_t *title, *username;
	image_t *input;
		
	Pixmap background;
};

theme_t* theme_new(display_t *display, cfg_t *conf);
void theme_delete(theme_t *theme);

#endif /* __THEME_H__  */
