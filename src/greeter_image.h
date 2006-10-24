#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <X11/Xlib.h>

#include "greeter_window.h"

XImage* image_load(display_t *display, const char *filename);
Pixmap image_to_pixmap(window_t *window, XImage *image, Pixmap pixmap);

#endif /* __IMAGE_H__  */
