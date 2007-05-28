#ifndef GUI_INPUT_H_
#define GUI_INPUT_H_

#include <X11/Xft/Xft.h>

typedef struct gui_input_t gui_input_t;

#include "display.h"
#include "gui_image.h"
#include "gui.h"

/**
 * Describes moving the cursor
 * to be relative to the current position.
 */
typedef enum pos_mode_t {
	INPUT_POS_REL,
	INPUT_POS_ABS,
	INPUT_POS_END
} pos_mode_t;

/**
 * This object represents a input box.
 */
struct gui_input_t {
	GUI_WIDGET_BASE;

	int t_x, t_y, t_w, t_h;
	int pos;
	int hidden;

	gui_image_t *image;
	gui_label_t *text;
};

/**
 * Create a new gui_input_t object.
 * @param display The display to use.
 * @param image A image to load as graphics.
 * @param x The initial position of the object.
 * @param y The initial position of the object.
 * @param font Which font to use for the text.
 * @param color Which color to use on the text.
 * @param text_x The area where text is allowed.
 * @param text_y The area where text is allowed.
 * @param text_w The area where text is allowed.
 * @param text_h The area where text is allowed.
 * @param hidden Whether the text should be visible or not.
 * @return A new gui_input_t object.
 */
gui_input_t* gui_input_new(display_t *display, const char *image, int x, int y,
		const char *font, const char *color, 
		int text_x, int text_y, int text_w, int text_h, int hidden);

/**
 * Deletes an object created by gui_input_new.
 * @param input The object to free.
 * @param display The display to use.
 */
void gui_input_delete(gui_input_t *input, display_t *display);

/**
 * Draws the object on a window.
 * @param input The object to draw.
 * @param gui The window to draw on.
 * @param focus Whether the control has focus or not.
 */
void gui_input_draw(gui_input_t *input, gui_t *gui, int focus);

/**
 * Return the text in the object.
 * @param input The object to fetch data from.
 * @return The text in input.
 */
char* gui_input_get_text(gui_input_t *input);

/**
 * Sets the text in a object.
 * @param input The object to assign data to.
 * @param text The text to assign.
 */
void gui_input_set_text(gui_input_t *input, const char *text);

/**
 * Sets the cursor position.
 * @param input The object to set the cursor on.
 * @param pos The new position.
 * @param mode Which mode to use when moving the cursor.
 */
void gui_input_set_pos(gui_input_t *input, int pos, pos_mode_t mode);

/**
 * Deletes a char at the current position.
 * @param input The input to use.
 */
void gui_input_delete_char(gui_input_t *input);

/**
 * Inserts a char at the current position.
 * @param input The object to add text to.
 * @param c The character to insert.
 */
void gui_input_insert_char(gui_input_t *input, char c);

#endif /*GUI_INPUT_H_*/
