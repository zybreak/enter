#ifndef GUI_LABEL_H_
#define GUI_LABEL_H_

#include <X11/Xft/Xft.h>

#define LABEL_TEXT_LEN 64

typedef struct gui_label_t gui_label_t;

#include "display.h"
#include "gui.h"

struct gui_label_t {
	int type;
	int x, y, w, h;

	XftFont *font;
	XftColor *color;
	char caption[LABEL_TEXT_LEN];
};

gui_label_t* gui_label_new(display_t *display, const char *font,
		const char *color, int x, int y, int w, int h,
		const char *caption);
void gui_label_delete(gui_label_t *label, display_t *display);
void gui_label_draw(gui_label_t *label, gui_t *gui);
char* gui_label_get_caption(gui_label_t *label);
void gui_label_set_caption(gui_label_t *label, const char *caption);

#endif /*GUI_LABEL_H_*/
