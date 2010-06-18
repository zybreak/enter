#ifndef GUI_WIDGET_H_
#define GUI_WIDGET_H_

#include "enter.h"

typedef union gui_widget_t gui_widget_t;

/**
 * The different types of widgets available.
 */
typedef enum gui_widget_type_t {
	LABEL,
	INPUT,
	IMAGE
} gui_widget_type_t;

/**
 * This is data common to all controls that inherit from gui_widget_t.
 */
#define GUI_WIDGET_BASE \
	struct { \
		int type; \
		int x, y, w, h; \
		action_t (*on_key_press)(gui_widget_t*, xcb_keysym_t); \
	}

#include "gui_label.h"
#include "gui_image.h"
#include "gui_input.h"

/**
 * This is the base control that all other controls inherit from.
 */
union gui_widget_t {
	GUI_WIDGET_BASE;

	gui_label_t label;
	gui_image_t image;
	gui_input_t input;
};

/**
 * Get the height of a widget.
 * @param widget The widget to fetch data from.
 * @return The height of a widget.
 */
int gui_widget_height(gui_widget_t *widget);

/**
 * Get the width of a widget.
 * @param widget The widget to fetch data from.
 * @return The width of a widget.
 */
int gui_widget_width(gui_widget_t *widget);

/**
 * Get the position of a widget.
 * @param widget The widget to fetch data from.
 * @return The position of the widget.
 */
int gui_widget_x(gui_widget_t *widget);

/**
 * Get the position of a widget.
 * @param widget The widget to fetch data from.
 * @return The position of the widget.
 */
int gui_widget_y(gui_widget_t *widget);

/**
 * Draw a widget to a window.
 * @param widget The widget to draw.
 * @param gui The window to draw on.
 */
void gui_widget_draw(gui_widget_t *widget, gui_t *gui);

/**
 * Draw a focused widget to a window.
 * @param widget The widget to draw.
 * @param gui The window to draw on.
 */
void gui_widget_draw_focus(gui_widget_t *widget, gui_t *gui);

/**
 * Delete a widget.
 * @param widget The widget to free.
 */
void gui_widget_delete(gui_widget_t *widget);

#endif /*GUI_WIDGET_H_*/
