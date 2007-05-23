#include <stdlib.h>

#include "enter.h"
#include "utils.h"

#include "gui_widget.h"

gui_image_t* gui_image_new(display_t *display, const char *filename,
							int x, int y)
{
	gui_image_t *image = xmalloc(sizeof(*image));

	image->display = display;
	image->x = x;
	image->y = y;
	image->im_image = NULL;
	image->w = image->h = 0;
	image->type = IMAGE;

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

static void gui_image_draw_drawable(gui_image_t *image, Drawable drawable)
{
	display_t *display = image->display;

	imlib_context_set_display(display->dpy);
	imlib_context_set_visual(display->visual);
	imlib_context_set_colormap(display->colormap);
	imlib_context_set_drawable(drawable);

	imlib_context_set_image(image->im_image);

	imlib_render_image_on_drawable(image->x,image->y);
}

void gui_image_draw(gui_image_t *image, gui_t *gui)
{
	gui_image_draw_drawable(image, gui->drawable);
}

Pixmap gui_image_pixmap(gui_image_t *image)
{
	display_t *display = image->display;

	Pixmap pixmap = XCreatePixmap(display->dpy, display->root,
			image->w, image->h, display->depth);

	int x = image->x;
	int y = image->y;

	image->x = image->y = 0;

	gui_image_draw_drawable(image, pixmap);

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

