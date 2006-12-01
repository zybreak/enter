#ifndef GUI_INPUT_H_
#define GUI_INPUT_H_

#include <X11/Xft/Xft.h>

typedef struct gui_input_t gui_input_t;

#include "display.h"
#include "gui_image.h"
#include "gui.h"

#define TEXT_LEN 64

#define gui_input_x(input) \
	((input)->x)
#define gui_input_y(input) \
	((input)->y)
#define gui_input_width(input) \
	((input)->w)
#define gui_input_height(input) \
	((input)->h)
#define gui_input_text(input) \
	((input)->text->caption)

struct gui_input_t {
	gui_image_t *image;
	int x, y, w, h;
	gui_label_t *text;
};

gui_input_t* gui_input_new(display_t *display, const char *image, int x, int y,
		const char *font, const char *color, 
		int text_x, int text_y, int text_w, int text_h);
void gui_input_delete(gui_input_t *input, display_t *display);
void gui_input_draw(gui_input_t *input, gui_t *gui, int hidden);
void gui_input_clear(gui_input_t *input, gui_t *gui);

#endif /*GUI_INPUT_H_*/
