#ifndef GUI_WIDGETS_H_
#define GUI_WIDGETS_H_

#include <X11/Xft/Xft.h>

typedef struct gui_input_t gui_input_t;
typedef struct gui_label_t gui_label_t;

#include "greeter_display.h"
#include "greeter_image.h"
#include "greeter_gui.h"

#define TEXT_LEN 64

struct gui_input_t {
	image_t *image;
	int x, y, w, h;
	int text_x, text_y, text_w, text_h;
	XftFont *font;
	XftColor *color;
	char text[TEXT_LEN];
};

struct gui_label_t {
	XftFont *font;
	XftColor *color;
	char caption[TEXT_LEN];
	int x,y;
};

gui_label_t* gui_label_new(display_t *display, const char *font,
		const char *color, int x, int y, const char *caption);
gui_input_t* gui_input_new(display_t *display, const char *image, int x, int y,
		const char *font, const char *color, 
		int text_x, int text_y, int text_w, int text_h);
void gui_label_delete(gui_label_t *label, display_t *display);
void gui_input_delete(gui_input_t *input, display_t *display);
void gui_label_draw(gui_label_t *label, gui_t *gui);
void gui_input_draw(gui_input_t *input, gui_t *gui, int hidden);

#endif /*GUI_WIDGETS_H_*/
