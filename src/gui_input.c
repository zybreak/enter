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
	gui_image_draw(gui->win, input->image);

	char old[TEXT_LEN];
	strncpy(old, gui_input_text(input), TEXT_LEN);

	if (hidden) {
		memset(gui_input_text(input), '*',
				strlen(gui_input_text(input)));
	}

	gui_label_draw(input->text, gui);

	strncpy(gui_input_text(input), old, TEXT_LEN);
}

void gui_input_clear(gui_input_t *input, gui_t *gui)
{
	display_t *display = gui->display;
	XClearArea(display->dpy, gui->win, input->x,
			input->y,
			input->w,
			input->h, False);
}

