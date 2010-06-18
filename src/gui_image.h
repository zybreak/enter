#ifndef __GUI_IMAGE_H__
#define __GUI_IMAGE_H__

#include <Imlib2.h>

#include "display.h"
#include "gui.h"
#include "gui_widget.h"

/**
 * This object represents an image.
 */
typedef struct gui_image_t {
	GUI_WIDGET_BASE;

	Imlib_Image im_image;
	display_t *display;
} gui_image_t;

/**
 * Creates an gui_image_t object.
 * @param display Which display to use.
 * @param filename Where to load the image from.
 * @param x The images initial position.
 * @param y The images initial position.
 * @return A new gui_image_t object.
 */
gui_image_t* gui_image_new(display_t *display,
		const char *filename, int x, int y);

/**
 * Delete a object created by gui_image_new.
 * @param image The object to free.
 */
void gui_image_delete(gui_image_t *image);

/**
 * Draw an image object.
 * @param image The image to draw.
 * @param gui The gui_t object to draw upon.
 */
void gui_image_draw(gui_image_t *image, gui_t *gui);

/**
 * Creates a Pixmap from a gui_image_t object.
 * @param image The image to convert.
 * @return The gui_image_t object as an Pixmap.
 */
xcb_pixmap_t gui_image_pixmap(gui_image_t *image);

/**
 * Load a image from a file to a gui_image_t object.
 * @param image The object to store the image in.
 * @param filename The image to load.
 * @return TRUE if successful, otherwise FALSE.
 */
int gui_image_load(gui_image_t *image, const char *filename);

#endif /* __GUI_IMAGE_H__  */
