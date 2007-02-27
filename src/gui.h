#ifndef GUI_H_
#define GUI_H_

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xdbe.h>

typedef struct gui_t gui_t;

#include "gui_label.h"
#include "gui_input.h"

#include "display.h"
#include "conf.h"

struct gui_t {
	Window win;
	Drawable drawable;
	XftDraw *draw;
	
	int has_doublebuf;
	XdbeBackBuffer back_buffer;
	XdbeSwapInfo swap_info;

	int x, y;
	int width, height;
	
	display_t *display;
	conf_t *conf;

	Pixmap background;
	gui_label_t *title, *username, *password, *msg;
	gui_input_t *user_input, *passwd_input;

	enum {
		LISTEN,
		LOGIN
	} mode;

	enum {
		BOTH,
		USERNAME,
		PASSWORD
	} visible, focus;
};

gui_t* gui_new(display_t *display, conf_t *theme);
void gui_delete(gui_t *gui);
void gui_show(gui_t *gui);
void gui_hide(gui_t *gui);
int gui_run(gui_t *gui);

#endif /*GUI_H_*/
