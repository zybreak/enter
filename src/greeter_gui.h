#ifndef GUI_H_
#define GUI_H_

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>

#include "greeter_display.h"
#include "conf.h"

#define TEXT_LEN 64

typedef struct gui_input_t {
	Pixmap image;
	int x, y, w, h;
	int text_x, text_y, text_w, text_h;
	XftFont *font;
	XftColor *color;
	char text[TEXT_LEN];
} gui_input_t;

typedef struct gui_label_t {
	XftFont *font;
	XftColor *color;
	char caption[TEXT_LEN];
	int x,y;
} gui_label_t;

typedef struct gui_t {
	Window win;
	Pixmap background;
	XftDraw *draw;
	int x, y;
	int width, height;
	display_t *display;

	gui_label_t *title, *username, *password;
	gui_input_t *user_input, *passwd_input;

	enum {
		ALL,
		USERNAME,
		PASSWORD
	} visible, focus;
} gui_t;

gui_t* gui_new(display_t *display, cfg_t *config);
void gui_delete(gui_t *gui);
void gui_show(gui_t *gui);
void gui_events(gui_t *gui, XEvent *event);

#endif /*GUI_H_*/
