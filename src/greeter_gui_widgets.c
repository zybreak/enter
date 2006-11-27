#include <X11/Xlib.h>

#include "greeter_gui_widgets.h"
#include "utils.h"

gui_label_t* gui_label_new(display_t *display, const char *font,
		const char *color, int x, int y, int w, int h, const char *caption)
{
	gui_label_t *label = xmalloc(sizeof(*label));
	XGlyphInfo extent;

	label->font = XftFontOpenName(display->dpy, display->screen, font);
	
	if (!label->font) {
		free(label);
		return NULL;
	}

	strncpy(label->caption,caption,TEXT_LEN-1);
	
	XftTextExtents8(display->dpy,label->font,(XftChar8*)"l",1,&extent);

	label->x = x;
	label->y = y+extent.height;
	label->w = w;
	label->h = h;

	label->color = xmalloc(sizeof(*label->color));
	XftColorAllocName(display->dpy,display->visual,display->colormap,
							color, label->color);
	
	return label;
}

gui_input_t* gui_input_new(display_t *display, const char *image, int x, int y,
		const char *font, const char *color, 
		int text_x, int text_y, int text_w, int text_h)
{
	int t_x, t_y, t_w, t_h;
	gui_input_t *input = xmalloc(sizeof(*input));

	input->x = x;
	input->y = y;

	input->image = gui_image_load(display, image);
	if (!input->image) {
		free(input);
		return NULL;
	}
	
	t_x = (text_x>0)?x+text_x:x;
	t_y = (text_y>0)?y+text_y:y;
	t_w = (text_w>0)?text_w:gui_image_width(input->image);
	t_h = (text_h>0)?text_h:gui_image_height(input->image);

	input->text = gui_label_new(display, font, color, t_x, t_y,
					t_w, t_h, "");
	if (!input->text) {
		gui_image_delete(input->image);
		free(input);
		return NULL;
	}

	return input;
}

void gui_label_delete(gui_label_t *label, display_t *display)
{
	XftFontClose(display->dpy,label->font);
	XftColorFree(display->dpy,display->visual,display->colormap,label->color);
	free(label->color);
	free(label);
}

void gui_input_delete(gui_input_t *input, display_t *display)
{
	gui_image_delete(input->image);
	gui_label_delete(input->text, display);
	free(input);
}

void gui_label_draw(gui_label_t *label, gui_t *gui)
{
	display_t *display = gui->display;
	XftDraw *draw = gui->draw;
	char *p;
	XGlyphInfo extents;

	p = label->caption;
	XftTextExtents8(display->dpy, label->font,
			(XftChar8*)p, strlen(p), &extents);

	int w = extents.xOff;

	/* If no width was specified, print everything.  */
	if (label->w == 0)
		w = -1;

	while (*p && w >= label->w) {
		XftTextExtents8(display->dpy, label->font,
				(XftChar8*)p, 1, &extents);
		w -= extents.xOff;
		p++;
	}

	XftDrawString8(draw,label->color,label->font, 
		label->x,label->y,
		(XftChar8*)p,strlen(p));
}

void gui_input_draw(gui_input_t *input, gui_t *gui, int hidden)
{
	gui_image_draw(gui->win, input->image, input->x, input->y);

	char old[TEXT_LEN];
	strncpy(old, gui_input_text(input), TEXT_LEN);

	if (hidden) {
		memset(gui_input_text(input), '*',
				strlen(gui_input_text(input)));
	}

	gui_label_draw(input->text, gui);

	strncpy(gui_input_text(input), old, TEXT_LEN);
}

