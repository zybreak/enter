#ifndef __THEME_H__
#define __THEME_H__

#include "enter.h"

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

typedef struct theme_t theme_t;
typedef union item_t item_t;

typedef enum e_type {
	IMAGE,
	LABEL
} e_type;

typedef struct label_t {
	e_type type;
	XftFont *font;
	XftColor *color;
	char *caption;
	int x, y;
} label_t;

typedef struct image_t {
	e_type type;
	Pixmap image;
	int x, y;
} image_t;

#include "greeter_display.h"
#include "cfg.h"

struct theme_t {
	display_t *display;

	union item_t {
		e_type type;
		label_t label;
		image_t image;
	} *title, *username, *input;
		
	Pixmap background;
};

theme_t* theme_new(display_t *display, cfg_t *conf);
void theme_delete(theme_t *theme);

#endif /* __THEME_H__  */
