#ifndef GUI_INPUT_H_
#define GUI_INPUT_H_

#include <X11/Xft/Xft.h>

typedef struct gui_input_t gui_input_t;

#include "display.h"
#include "gui_image.h"
#include "gui.h"

typedef enum {
	INPUT_POS_REL,
	INPUT_POS_ABS
} pos_mode_t;

struct gui_input_t {
	GUI_WIDGET_BASE;

	int t_x, t_y, t_w, t_h;
	int pos;
	int hidden;

	gui_image_t *image;
	gui_label_t *text;
};

gui_input_t* gui_input_new(display_t *display, const char *image, int x, int y,
		const char *font, const char *color, 
		int text_x, int text_y, int text_w, int text_h, int hidden);
void gui_input_delete(gui_input_t *input, display_t *display);
void gui_input_draw(gui_input_t *input, gui_t *gui, int draw_cursor);
char* gui_input_get_text(gui_input_t *input);
void gui_input_set_text(gui_input_t *input, const char *text);
void gui_input_set_pos(gui_input_t *input, int pos, pos_mode_t mode);
void gui_input_delete_char(gui_input_t *input);
void gui_input_insert_char(gui_input_t *input, char c);

#endif /*GUI_INPUT_H_*/
