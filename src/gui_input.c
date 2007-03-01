#include <X11/Xlib.h>

#include "gui_input.h"
#include "utils.h"

gui_input_t* gui_input_new(display_t *display, const char *image, int x, int y,
		const char *font, const char *color, 
		int text_x, int text_y, int text_w, int text_h)
{
	int t_x, t_y, t_w, t_h;
	gui_input_t *input = xmalloc(sizeof(*input));

	input->x = x;
	input->y = y;

	input->image = gui_image_new(display, image, x, y);
	if (!input->image) {
		free(input);
		return NULL;
	}
	
	t_x = (text_x>0)?x+text_x:x;
	t_y = (text_y>0)?y+text_y:y;
	t_w = (text_w>0)?text_w:gui_image_width(input->image);
	t_h = (text_h>0)?text_h:gui_image_height(input->image);

	input->pos = 0;
	input->text = gui_label_new(display, font, color, t_x, t_y,
					t_w, t_h, "");
	if (!input->text) {
		gui_image_delete(input->image);
		free(input);
		return NULL;
	}

	input->w = gui_image_width(input->image);
	input->h = gui_image_height(input->image);

	return input;
}

void gui_input_delete(gui_input_t *input, display_t *display)
{
	gui_image_delete(input->image);
	gui_label_delete(input->text, display);
	free(input);
}

void gui_input_draw(gui_input_t *input, gui_t *gui, int hidden)
{
	gui_image_draw(gui->drawable, input->image);

	char old[LABEL_TEXT_LEN];
	strncpy(old, gui_input_get_text(input), LABEL_TEXT_LEN);

	if (hidden) {
		memset(gui_input_get_text(input), '*',
				strlen(gui_input_get_text(input)));
	}

	gui_label_draw(input->text, gui);

	gui_input_set_text(input, old);
}

char* gui_input_get_text(gui_input_t *input)
{
	return gui_label_get_caption(input->text);
}

void gui_input_set_text(gui_input_t *input, const char *text)
{
	gui_label_set_caption(input->text, text);
	gui_input_set_pos(input, strlen(text), INPUT_POS_ABS);
}

void gui_input_set_pos(gui_input_t *input, int pos, pos_mode_t mode)
{
	if (mode == INPUT_POS_REL)
		input->pos += pos;
	else
		input->pos = pos;

	if (input->pos > strlen(gui_input_get_text(input)))
		input->pos = strlen(gui_input_get_text(input));

	if (input->pos < 0)
		input->pos = 0;
}

void gui_input_delete_char(gui_input_t *input)
{
	gui_input_set_pos(input, -1, INPUT_POS_REL);

	xstrdel(gui_label_get_caption(input->text),input->pos);
}

void gui_input_insert_char(gui_input_t *input, char c)
{
	xstrins(gui_label_get_caption(input->text), input->pos, c, LABEL_TEXT_LEN);

	gui_input_set_pos(input, 1, INPUT_POS_REL);
}

int gui_input_x(gui_input_t *input)
{
	return input->x;
}

int gui_input_y(gui_input_t *input)
{
	return input->y;
}

int gui_input_width(gui_input_t *input)
{
	return input->w;
}

int gui_input_height(gui_input_t *input)
{
	return input->h;
}

