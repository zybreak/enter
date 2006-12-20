#include "gui_widget.h"

int gui_widget_height(gui_widget_t *widget)
{
	return widget->widget.h;
}

int gui_widget_width(gui_widget_t *widget)
{
	return widget->widget.w;
}

int gui_widget_x(gui_widget_t *widget)
{
	return widget->widget.x;
}

int gui_widget_y(gui_widget_t *widget)
{
	return widget->widget.y;
}

