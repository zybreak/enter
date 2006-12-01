#ifndef GUI_LABEL_H_
#define GUI_LABEL_H_

#include <X11/Xft/Xft.h>

typedef struct gui_label_t gui_label_t;

#include "display.h"
#include "gui.h"

#define TEXT_LEN 64

#define gui_label_set_caption(label, str) \
	strncpy((label)->caption, (str), TEXT_LEN-1)

struct gui_label_t {
	XftFont *font;
	XftColor *color;
	char caption[TEXT_LEN];
	int x, y, w, h;
};

gui_label_t* gui_label_new(display_t *display, const char *font,
		const char *color, int x, int y, int w, int h,
		const char *caption);
void gui_label_delete(gui_label_t *label, display_t *display);
void gui_label_draw(gui_label_t *label, gui_t *gui);
void gui_label_clear(gui_label_t *label, gui_t *gui);

#endif /*GUI_LABEL_H_*/
