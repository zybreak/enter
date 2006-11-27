#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <X11/Xlib.h>

#include "greeter_display.h"

typedef struct image_t image_t;

void image_delete(image_t *image);
int image_width(image_t *image);
int image_height(image_t *image);
void image_draw(Drawable drawable, image_t *image, int x, int y);
Pixmap image_pixmap(image_t *image);
image_t* image_load(display_t *display, const char *filename);

#endif /* __IMAGE_H__  */
