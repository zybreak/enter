#ifndef GUI_LABEL_H_
#define GUI_LABEL_H_

#include <xcb/xcb.h>

typedef struct gui_label_t gui_label_t;

#include "display.h"
#include "gui.h"

/**
 * This object represents a label.
 */
struct gui_label_t {
	GUI_WIDGET_BASE;

	xcb_gcontext_t gc;
	char *caption;
};

/**
 * Create a new gui_label_t object.
 * @param display The display to use.
 * @param font The font to use.
 * @param color The color to use.
 * @param x Initial position.
 * @param y Initial position.
 * @param w Initial dimension.
 * @param h Initial dimension.
 * @param caption Caption of the label.
 * @return A new gui_label_t object.
 */
gui_label_t* gui_label_new(display_t *display, const char *font_name,
		const char *color, int x, int y, int w, int h,
		const char *caption);

/**
 * Deletes an object created by gui_label_new.
 * @param label The object to free.
 * @param display The display to use.
 */
void gui_label_delete(gui_label_t *label, display_t *display);

/**
 * Draw a label on a window.
 * @param label The label to draw.
 * @param gui The window to draw upon.
 */
void gui_label_draw(gui_label_t *label, gui_t *gui);

/**
 * Return the current label of a caption.
 * @param label The label to fetch data from.
 * @return The caption of the label.
 */
char* gui_label_get_caption(gui_label_t *label);

/**
 * Assign a new caption to a label.
 * @param label The label to assign data to.
 * @param caption The new caption.
 */
void gui_label_set_caption(gui_label_t *label, const char *caption);

#endif /*GUI_LABEL_H_*/
