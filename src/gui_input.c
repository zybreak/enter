#include <X11/Xlib.h>

#include "utils.h"

#include "gui_widget.h"

gui_input_t* gui_input_new(display_t *display, const char *image, int x, int y,
		const char *font, const char *color, 
		int text_x, int text_y, int text_w, int text_h, int hidden)
{
	gui_input_t *input = xmalloc(sizeof(*input));

	input->x = x;
	input->y = y;
	input->type = INPUT;
	input->hidden = hidden;

	input->image = gui_image_new(display, image, x, y);
	if (!input->image) {
		free(input);
		return NULL;
	}
	
	/* Assign t_* the value of their counterpart if
	 * a value below zero was specified.  */
	input->t_x = (text_x>0)?x+text_x:x;
	input->t_y = (text_y>0)?y+text_y:y;
	input->t_w = (text_w>0)?text_w:gui_widget_width((gui_widget_t*)input->image);
	input->t_h = (text_h>0)?text_h:gui_widget_height((gui_widget_t*)input->image);

	input->pos = 0;
	input->text = gui_label_new(display, font, color,
					input->t_x, input->t_y,
					input->t_w, input->t_h, "");
	if (!input->text) {
		gui_image_delete(input->image);
		free(input);
		return NULL;
	}

	input->w = gui_widget_width((gui_widget_t*)input->image);
	input->h = gui_widget_height((gui_widget_t*)input->image);

	return input;
}

void gui_input_delete(gui_input_t *input, display_t *display)
{
	gui_image_delete(input->image);
	gui_label_delete(input->text, display);
	free(input);
}

void gui_input_draw(gui_input_t *input, gui_t *gui, int draw_cursor)
{
	char buf[LABEL_TEXT_LEN];
	char *str = gui_label_get_caption(input->text);
	int i = 0;

	strncpy(buf, str, LABEL_TEXT_LEN-1);

	if (input->hidden) {
		for (i = 0; i < strlen(str); i++)
			str[i] = '*';
	}
	
	gui_image_draw(gui->drawable, input->image);

	gui_label_draw(input->text, gui);
	
	if (draw_cursor) {
		XGlyphInfo extents;
		display_t *display = gui->display;

		XftTextExtents8(display->dpy, input->text->font,
				(XftChar8*)str, input->pos, &extents);

		int x = input->t_x + extents.width;

		if (x > input->x + input->t_w) {
			x = input->x + input->t_w;
		}

		XDrawLine(display->dpy, gui->drawable, display->gc,
			x, input->t_y, x, input->t_y + input->t_h);
	}
	
	strncpy(str, buf, LABEL_TEXT_LEN-1);
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

	if (input->pos < 0)
		input->pos = 0;
	else if (input->pos > strlen(gui_input_get_text(input)))
		input->pos = strlen(gui_input_get_text(input));
}

void gui_input_delete_char(gui_input_t *input)
{
	if (input->pos == 0)
		return;

	gui_input_set_pos(input, -1, INPUT_POS_REL);

	xstrdel(gui_label_get_caption(input->text),input->pos);
}

void gui_input_insert_char(gui_input_t *input, char c)
{
	xstrins(gui_label_get_caption(input->text), input->pos, c, LABEL_TEXT_LEN);

	gui_input_set_pos(input, 1, INPUT_POS_REL);
}

