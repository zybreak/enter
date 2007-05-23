#ifndef GUI_H_
#define GUI_H_

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xdbe.h>

typedef struct gui_t gui_t;

#include "gui_widget.h"
#include "display.h"
#include "list.h"

struct gui_t {
	display_t *display;
	Window win;
	Drawable drawable;
	XftDraw *draw;
	
	XdbeBackBuffer back_buffer;
	XdbeSwapInfo swap_info;

	int x, y;
	int width, height;

	list_t *widgets;
	gui_widget_t *focus;
};

gui_t* gui_new(display_t *display);
void gui_delete(gui_t *gui);
void gui_show(gui_t *gui);
void gui_hide(gui_t *gui);
void gui_handle_event(gui_t *gui, XEvent *event);

#endif /*GUI_H_*/
