#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <X11/Xlib.h>

#include "greeter_display.h"

Pixmap image_load(display_t *display, const char *filename);

#endif /* __IMAGE_H__  */
