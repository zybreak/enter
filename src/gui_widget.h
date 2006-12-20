#ifndef GUI_WIDGET_H_
#define GUI_WIDGET_H_

#include "gui_label.h"
#include "gui_image.h"
#include "gui_input.h"

typedef union gui_widget_t {
	struct {
		int type;
		int x, y, w, h;
	} widget;

	gui_label_t label;
	gui_image_t image;
	gui_input_t input;
} gui_widget_t;

int gui_widget_height(gui_widget_t *widget);
int gui_widget_width(gui_widget_t *widget);
int gui_widget_x(gui_widget_t *widget);
int gui_widget_y(gui_widget_t *widget);
void gui_widget_draw(gui_widget_t *widget);

#endif /*GUI_WIDGET_H_*/
