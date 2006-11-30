#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <X11/Xlib.h>

#include "display.h"

typedef struct gui_image_t gui_image_t;

void gui_image_delete(gui_image_t *image);
int gui_image_width(gui_image_t *image);
int gui_image_height(gui_image_t *image);
void gui_image_draw(Drawable drawable, gui_image_t *image, int x, int y);
Pixmap gui_image_pixmap(gui_image_t *image);
gui_image_t* gui_image_load(display_t *display, const char *filename);

#endif /* __IMAGE_H__  */
