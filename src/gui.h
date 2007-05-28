#ifndef GUI_H_
#define GUI_H_

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xdbe.h>

typedef struct gui_t gui_t;

#include "gui_widget.h"
#include "display.h"
#include "list.h"

/**
 * The object responsible for low level GUI routines.
 * Such as, how to redraw the widgets, and how to handle events.
 */
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

/**
 * Creates a new gui_t object.
 * @param display Which display to use.
 * @return A new gui_t object.
 */
gui_t* gui_new(display_t *display);

/**
 * Deletes the object allocated by gui_new.
 * @param gui The gui_t object to free.
 */
void gui_delete(gui_t *gui);

/**
 * Shows the gui_t window on the display.
 * @param gui Which gui_t object to use.
 */
void gui_show(gui_t *gui);

/**
 * Hides the gui_t window from the display.
 * @param gui Which gui_t object to use.
 */
void gui_hide(gui_t *gui);

/**
 * Handle event.
 * @param gui Which gui_t object to use.
 * @param event What event to handle.
 */
void gui_handle_event(gui_t *gui, XEvent *event);

#endif /*GUI_H_*/
