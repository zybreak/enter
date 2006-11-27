#include <X11/Xlib.h>
#include <Imlib2.h>

#include <stdio.h>
#include <stdlib.h>

#include "enter.h"
#include "greeter_image.h"
#include "utils.h"

struct image_t {
	int width;
	int height;
	Imlib_Image im_image;
	display_t *display;
};

void image_delete(image_t *image)
{
	imlib_context_set_image(image->im_image);
	imlib_free_image();
	free(image);
}

int image_width(image_t *image)
{
	return image->width;
}

int image_height(image_t *image)
{
	return image->height;
}

void image_draw(Drawable drawable, image_t *image, int x, int y)
{
	display_t *display = image->display;

	imlib_context_set_display(display->dpy);
	imlib_context_set_visual(display->visual);
	imlib_context_set_colormap(display->colormap);
	imlib_context_set_drawable(drawable);

	imlib_context_set_image(image->im_image);

	imlib_render_image_on_drawable(x,y);
}

Pixmap image_pixmap(image_t *image)
{
	display_t *display = image->display;

	Pixmap pixmap = XCreatePixmap(display->dpy, display->root,
			image->width, image->height, display->depth);

	image_draw(pixmap, image, 0, 0);

	return pixmap;
}

image_t* image_load(display_t *display, const char *filename)
{
	image_t *image = xmalloc(sizeof(*image));

	image->im_image = imlib_load_image(filename);
	if (!image->im_image) {
		free(image);
		return NULL;
	}

	image->display = display;

	imlib_context_set_image(image->im_image);
	image->width = imlib_image_get_width();
	image->height = imlib_image_get_height();

	return image;
}

