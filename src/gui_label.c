#include <X11/Xlib.h>

#include "utils.h"

#include "gui_label.h"

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

	memset(label->caption, '\0', LABEL_TEXT_LEN);
	strncpy(label->caption,caption,LABEL_TEXT_LEN-1);
	
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

void gui_label_delete(gui_label_t *label, display_t *display)
{
	XftFontClose(display->dpy,label->font);
	XftColorFree(display->dpy,display->visual,display->colormap,label->color);
	free(label->color);
	free(label);
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

char* gui_label_get_caption(gui_label_t *label)
{
	return label->caption;
}

void gui_label_set_caption(gui_label_t *label, const char *caption)
{
	strncpy(label->caption, caption, LABEL_TEXT_LEN-1);
}

