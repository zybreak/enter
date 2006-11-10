#include "greeter_gui_widgets.h"
#include "greeter_image.h"
#include "utils.h"

gui_label_t* gui_label_new(display_t *display, const char *font,
		const char *color, int x, int y, const char *caption)
{
	gui_label_t *label = xmalloc(sizeof(*label));
	XGlyphInfo extent;

	label->x = x;
	strncpy(label->caption,caption,TEXT_LEN-1);
	
	label->font = XftFontOpenName(display->dpy, display->screen, font);
	
	if (!label->font) {
		free(label);
		return NULL;
	}

	XftTextExtents8(display->dpy,label->font,(XftChar8*)"l",1,&extent);

	label->y = y-extent.height;
	
	label->color = xmalloc(sizeof(*label->color));
	XftColorAllocName(display->dpy,display->visual,display->colormap,
							color, label->color);
	
	return label;
}

gui_input_t* gui_input_new(display_t *display, const char *image, int x, int y,
		const char *font, const char *color, 
		int text_x, int text_y, int text_w, int text_h)
{
	gui_input_t *input = xmalloc(sizeof(*input));
	XGlyphInfo extent;

	input->x = x;
	memset(input->text,'\0',TEXT_LEN);

	/* If no text_x or text_x was supplied, use the x/y
	 * values of the input.  */	
	input->text_x = (text_x>0)?text_x:x;
	input->text_y = (text_y>0)?text_y:y;
	
	memset(input->text,'\0',TEXT_LEN);

	input->image = image_load(display,image,&(input->w),&(input->h));

	if (!input->image) {
		free(input);
		return NULL;
	}

	/* If no text_w or text_h was supplied, use the width/height
	 * values of the image.  */	
	input->text_w = (text_w>0)?text_w:input->w;
	input->text_h = (text_h>0)?text_h:input->h;
	
	input->font = XftFontOpenName(display->dpy, display->screen, font);
	
	if (!input->font) {
		XFreePixmap(display->dpy, input->image);
		free(input);
		return NULL;
	}
	
	XftTextExtents8(display->dpy,input->font,(XftChar8*)"l",1,&extent);

	input->y = y-extent.height;
	
	input->color = xmalloc(sizeof(*input->color));
	XftColorAllocName(display->dpy,display->visual,display->colormap,
							color, input->color);

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
	XFreePixmap(display->dpy,input->image);
	XftFontClose(display->dpy,input->font);
	XftColorFree(display->dpy,display->visual,display->colormap,input->color);
	free(input->color);
	free(input);
}

void gui_label_draw(gui_label_t *label, gui_t *gui)
{
	XftDraw *draw = gui->draw;

	XftDrawString8(draw,label->color,label->font, 
		label->x,label->y,
		(XftChar8*)label->caption,strlen(label->caption));
}

void gui_input_draw(gui_input_t *input, gui_t *gui, int hidden)
{
	display_t *display = gui->display;
	XftDraw *draw = gui->draw;
	XGlyphInfo extents;
	int i;
	int len = strlen(input->text);
	char text[len+1];
	char *p;
	
	XCopyArea(display->dpy, input->image, gui->win, display->gc,
			0, 0, input->w, input->h, input->x,input->y);

	for (i=0;i<len;i++) {
		/* Hide the text when hidden is true,
		 * as to not display any passwords.  */
		text[i] = (hidden)?'*':input->text[i];
	}
	text[len] = '\0';

	XftTextExtents8(display->dpy,input->font,(XftChar8*)"W",1,&extents);

	p = text+len;
	int w = 0;

	/* Only print the text that fits in the box.  */
	while (p != text) {
		XftTextExtents8(display->dpy,input->font,
				(XftChar8*)&p[0],1,&extents);
		w += extents.xOff;
		if (w > input->text_w)
			break;
		p--;
	}

	XftDrawString8(draw,input->color,input->font, 
		input->text_x,input->text_y,
		(XftChar8*)p,strlen(p));
}

