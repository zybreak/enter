#ifndef GUI_WIDGET_H_
#define GUI_WIDGET_H_

typedef union gui_widget_t gui_widget_t;

#include "gui_label.h"
#include "gui_image.h"
#include "gui_input.h"

typedef enum gui_widget_type_t {
	LABEL,
	INPUT,
	IMAGE
} gui_widget_type_t;

union gui_widget_t {
	struct {
		int type;
		int x, y, w, h;
	} widget;

	gui_label_t label;
	gui_image_t image;
	gui_input_t input;
};

int gui_widget_height(gui_widget_t *widget);
int gui_widget_width(gui_widget_t *widget);
int gui_widget_x(gui_widget_t *widget);
int gui_widget_y(gui_widget_t *widget);
void gui_widget_draw(gui_widget_t *widget, gui_t *gui);
void gui_widget_draw_focus(gui_widget_t *widget, gui_t *gui);

#endif /*GUI_WIDGET_H_*/
