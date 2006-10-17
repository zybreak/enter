#ifndef __THEME_H__
#define __THEME_H__

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include "display.h"
#include "cfg.h"

typedef struct theme_t {
	XftFont *title;
	int title_x, title_y;
	char *title_caption;
	
	XImage *background;
} theme_t;

theme_t* theme_new(display_t *display, cfg_t *conf);

#endif /* __THEME_H__  */
