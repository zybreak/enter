#ifndef GUI_H_
#define GUI_H_

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

typedef struct gui_t gui_t;

#include "gui_label.h"
#include "gui_input.h"

#include "display.h"
#include "conf.h"
#include "greeter.h"

struct gui_t {
	Window win;
	Pixmap background;
	XftDraw *draw;
	int x, y;
	int width, height;
	
	display_t *display;
	cfg_t *conf;
	greeter_t *greeter;

	gui_label_t *title, *username, *password, *msg;
	gui_input_t *user_input, *passwd_input;

	enum {
		BOTH,
		USERNAME,
		PASSWORD
	} visible, focus;
};

gui_t* gui_new(greeter_t *greeter, display_t *display, cfg_t *config);
void gui_delete(gui_t *gui);
void gui_show(gui_t *gui);
void gui_hide(gui_t *gui);
void gui_events(gui_t *gui, XEvent *event);

#endif /*GUI_H_*/
