#include <X11/Xlib.h>
#include <Imlib2.h>

#include <stdio.h>
#include <stdlib.h>

#include "enter.h"
#include "gui_image.h"
#include "utils.h"

struct gui_image_t {
	int type;
	int x, y, w, h;

	Imlib_Image im_image;
	display_t *display;
};

gui_image_t* gui_image_new(display_t *display, const char *filename, int x, int y)
{
	gui_image_t *image = xmalloc(sizeof(*image));
	image->display = display;
	image->x = x;
	image->y = y;
	image->im_image = NULL;
	image->w = image->h = 0;

	if (filename) {
		if (gui_image_load(image, filename) == FALSE) {
			free(image);
			return NULL;
		}
	}

	return image;
}

void gui_image_delete(gui_image_t *image)
{
	imlib_context_set_image(image->im_image);
	imlib_free_image();
	free(image);
}

int gui_image_width(gui_image_t *image)
{
	return image->w;
}

int gui_image_height(gui_image_t *image)
{
	return image->h;
}

void gui_image_draw(Drawable drawable, gui_image_t *image)
{
	display_t *display = image->display;

	imlib_context_set_display(display->dpy);
	imlib_context_set_visual(display->visual);
	imlib_context_set_colormap(display->colormap);
	imlib_context_set_drawable(drawable);

	imlib_context_set_image(image->im_image);

	imlib_render_image_on_drawable(image->x,image->y);
}

Pixmap gui_image_pixmap(gui_image_t *image)
{
	display_t *display = image->display;

	Pixmap pixmap = XCreatePixmap(display->dpy, display->root,
			image->w, image->h, display->depth);

	/* TODO: recode when `gui_image_move' is added.  */
	int x = image->x;
	int y = image->y;
	image->x = image->y = 0;

	gui_image_draw(pixmap, image);

	image->x = x;
	image->y = y;

	return pixmap;
}

int gui_image_load(gui_image_t *image, const char *filename)
{
	image->im_image = imlib_load_image(filename);
	if (!image->im_image) {
		return FALSE;
	}

	imlib_context_set_image(image->im_image);
	image->w = imlib_image_get_width();
	image->h = imlib_image_get_height();

	return TRUE;
}

