#ifndef __GUI_IMAGE_H__
#define __GUI_IMAGE_H__

#include <X11/Xlib.h>
#include <Imlib2.h>

#include "display.h"

typedef struct gui_image_t {
	int type;
	int x, y, w, h;

	Imlib_Image im_image;
	display_t *display;
} gui_image_t;

gui_image_t* gui_image_new(display_t *display, const char *filename, int x, int y);
void gui_image_delete(gui_image_t *image);
int gui_image_width(gui_image_t *image);
int gui_image_height(gui_image_t *image);
void gui_image_draw(Drawable drawable, gui_image_t *image);
Pixmap gui_image_pixmap(gui_image_t *image);
int gui_image_load(gui_image_t *image, const char *filename);

#endif /* __GUI_IMAGE_H__  */
