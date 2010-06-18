#include <stdlib.h>
#include <xcb/xcb_image.h>

#include "enter.h"

#include "utils.h"
#include "gui_widget.h"

gui_image_t* gui_image_new(display_t *display, const char *filename,
							int x, int y)
{
	gui_image_t *image = g_new(gui_image_t,1);

	image->display = display;
	image->x = x;
	image->y = y;
	image->im_image = NULL;
	image->w = image->h = 0;
	image->type = IMAGE;

	if (filename) {
		if (gui_image_load(image, filename) == FALSE) {
			g_free(image);
			return NULL;
		}
	}

	return image;
}

void gui_image_delete(gui_image_t *image)
{
	imlib_context_set_image(image->im_image);
	imlib_free_image();
	g_free(image);
}

static void gui_image_draw_drawable(gui_image_t *image, xcb_drawable_t img)
{
	display_t *display = image->display;

	imlib_context_set_image(image->im_image);

	uint32_t *data = imlib_image_get_data_for_reading_only();

	int width = imlib_image_get_width();
	int height = imlib_image_get_height();
	int y,x;

	for(y = 0; y < height; y++) {
		for (x = 0;x<width;x++) {
			int i, pixel, tmp;

			i = y*width+x;

			tmp  = (data[i] >> 16) & 0xff;
			tmp += (data[i] >> 8)  & 0xff;
			tmp += data[i]         & 0xff;

			pixel = (tmp / 3 < 127) ? 0 : 1;

			//xcb_image_put_pixel(img,x,y,pixel);
		}
	}
}

void gui_image_draw(gui_image_t *image, gui_t *gui)
{
	gui_image_draw_drawable(image, gui->drawable);
}

xcb_pixmap_t gui_image_pixmap(gui_image_t *image)
{
	display_t *display = image->display;

	xcb_pixmap_t pixmap = xcb_generate_id(display->dpy);

	xcb_create_pixmap(display->dpy,
			display->screen->root_depth,
			pixmap,
			display->screen->root,
			display->screen->width_in_pixels,
			display->screen->height_in_pixels);
		
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

