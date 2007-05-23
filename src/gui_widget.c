#include "enter.h"
#include "gui_widget.h"

int gui_widget_height(gui_widget_t *widget)
{
	return widget->h;
}

int gui_widget_width(gui_widget_t *widget)
{
	return widget->w;
}

int gui_widget_x(gui_widget_t *widget)
{
	return widget->x;
}

int gui_widget_y(gui_widget_t *widget)
{
	return widget->y;
}

void gui_widget_draw(gui_widget_t *widget, gui_t *gui)
{
	switch (widget->type) {
	case LABEL:
		gui_label_draw(&widget->label, gui);
		break;
	case INPUT:
		gui_input_draw(&widget->input, gui, FALSE);
		break;
	case IMAGE:
	break;
	}
}

void gui_widget_draw_focus(gui_widget_t *widget, gui_t *gui)
{
	switch (widget->type) {
	case LABEL:
		gui_label_draw(&widget->label, gui);
		break;
	case INPUT:
		gui_input_draw(&widget->input, gui, TRUE);
		break;
	case IMAGE:
	break;
	}
}

void gui_widget_delete(gui_widget_t *widget)
{
	/* TODO: implement me.  */
}

