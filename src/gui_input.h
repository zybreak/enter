#ifndef GUI_INPUT_H_
#define GUI_INPUT_H_

#include <X11/Xft/Xft.h>

typedef struct gui_input_t gui_input_t;

#include "display.h"
#include "gui_image.h"
#include "gui.h"

gui_input_t* gui_input_new(display_t *display, const char *image, int x, int y,
		const char *font, const char *color, 
		int text_x, int text_y, int text_w, int text_h);
void gui_input_delete(gui_input_t *input, display_t *display);
void gui_input_draw(gui_input_t *input, gui_t *gui, int hidden);
char* gui_input_get_text(gui_input_t *input);
void gui_input_set_text(gui_input_t *input, const char *text);
int gui_input_x(gui_input_t *input);
int gui_input_y(gui_input_t *input);
int gui_input_width(gui_input_t *input);
int gui_input_height(gui_input_t *input);

#endif /*GUI_INPUT_H_*/
