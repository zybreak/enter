#include <string.h>

#include "enter.h"

#include "utils.h"
#include "gui_widget.h"

#define LABEL_TEXT_LEN 64

gui_label_t* gui_label_new(display_t *display, const char *font_name,
		const char *color, int x, int y, int w, int h, const char *caption)
{
	gui_label_t *label = g_new(gui_label_t,1);

	label->caption = g_new(char,LABEL_TEXT_LEN);
	strncpy(label->caption,caption,LABEL_TEXT_LEN-1);
	
	xcb_font_t font = xcb_generate_id(display->dpy);
	xcb_open_font(display->dpy, font, strlen(font_name), font_name);

	uint32_t value[] = {
		display->screen->black_pixel,
		font
	};

	label->gc = xcb_generate_id(display->dpy);
	xcb_create_gc(display->dpy, label->gc, display->screen->root,
			XCB_GC_FOREGROUND | XCB_GC_FONT, value);

	xcb_close_font(display->dpy,font);

	label->x = x;
	label->y = y+16;
	label->w = w;
	label->h = h;
	label->type = LABEL;

	return label;
}

void gui_label_delete(gui_label_t *label, display_t *display)
{
	g_free(label->caption);
	/* TODO: Free GC */
	g_free(label);
}

void gui_label_draw(gui_label_t *label, gui_t *gui)
{
	xcb_image_text_8(
			gui->display->dpy,
			strlen(label->caption),
			gui->drawable,
			label->gc,
			label->x,
			label->y,
			label->caption);
}

char* gui_label_get_caption(gui_label_t *label)
{
	return label->caption;
}

void gui_label_set_caption(gui_label_t *label, const char *caption)
{
	strncpy(label->caption, caption, LABEL_TEXT_LEN-1);
}

