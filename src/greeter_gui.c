#include <stdlib.h>

#include "enter.h"
#include "greeter_gui.h"
#include "greeter_image.h"
#include "utils.h"

#define BUF_LEN 64

static gui_label_t* gui_label_new(display_t *display, const char *font,
		const char *color, int x, int y, const char *caption)
{
	gui_label_t *label = xmalloc(sizeof(*label));

	label->x = x;
	label->y = y;
	strncpy(label->caption,caption,TEXT_LEN-1);
	
	label->font = XftFontOpenName(display->dpy, display->screen, font);
	
	if (!label->font) {
		free(label);
		return NULL;
	}
	
	label->color = xmalloc(sizeof(*label->color));
	XftColorAllocName(display->dpy,display->visual,display->colormap,
							color, label->color);
	
	return label;
}

static gui_input_t* gui_input_new(display_t *display, const char *image, int x, int y,
		const char *font, const char *color, 
		int text_x, int text_y, int text_w, int text_h)
{
	gui_input_t *input = xmalloc(sizeof(*input));

	input->x = x;
	input->y = y;
	input->text_x = text_x;
	input->text_y = text_y;
	input->text_w = text_w;
	input->text_h = text_h;
	memset(input->text,'\0',TEXT_LEN);

	input->image = image_load(display,image,&(input->w),&(input->h));

	if (!input->image) {
		free(input);
		return NULL;
	}
	
	input->font = XftFontOpenName(display->dpy, display->screen, font);
	
	if (!input->font) {
		XFreePixmap(display->dpy, input->image);
		free(input);
		return NULL;
	}
	
	input->color = xmalloc(sizeof(*input->color));
	XftColorAllocName(display->dpy,display->visual,display->colormap,
							color, input->color);

	return input;
}

static void gui_label_delete(gui_label_t *label, display_t *display)
{
	XftColorFree(display->dpy,display->visual,display->colormap,label->color);
	free(label);
}

static void gui_input_delete(gui_input_t *input, display_t *display)
{
	XFreePixmap(display->dpy,input->image);
	XftColorFree(display->dpy,display->visual,display->colormap,input->color);
	free(input);
}

static void gui_label_draw(gui_label_t *label, gui_t *gui)
{
	XftDraw *draw = gui->draw;

	XftDrawString8(draw,label->color,label->font, 
		label->x,label->y,
		(XftChar8*)label->caption,strlen(label->caption));
}

static void gui_input_draw(gui_input_t *input, gui_t *gui)
{
	display_t *display = gui->display;
	XftDraw *draw = gui->draw;

	XCopyArea(display->dpy, input->image, gui->win, display->gc,
			0, 0, input->w, input->h, input->x,input->y);
	
	XftDrawString8(draw,input->color,input->font, 
		input->text_x,input->text_y,
		(XftChar8*)input->text,strlen(input->text));
}

static void gui_draw(gui_t *gui)
{
	display_t *display = gui->display;
	
	XClearWindow(display->dpy,gui->win);
	
	gui_label_draw(gui->title, gui);
	gui_label_draw(gui->username, gui);
	gui_input_draw(gui->user_input, gui);

	XFlush(display->dpy);
}

static void gui_keypress(gui_t *gui, XEvent *event)
{
	char ch;
	KeySym keysym;
	XComposeStatus cstatus;

	XLookupString(&event->xkey,&ch,1,&keysym,&cstatus);

	if (keysym == XK_BackSpace) {
		int len = strlen(gui->user_input->text);
		if (len > 0)
			gui->user_input->text[len-1] = '\0';
	} else {
		strncat(gui->user_input->text,&ch,TEXT_LEN-1);
	}

	gui_draw(gui);
}

gui_t* gui_new(display_t *display, cfg_t *conf)
{
	gui_t *gui = xmalloc(sizeof(*gui));
	char buf[BUF_LEN];
	int w, h;

	memset(gui,'\0',sizeof(*gui));
	gui->x = 0;
	gui->y = 0;
	gui->width = display->width;
	gui->height = display->height;
	gui->display = display;

	unsigned long color = BlackPixel(display->dpy,display->screen);

	gui->win = XCreateSimpleWindow(display->dpy, display->root,
		gui->x, gui->y, gui->width, gui->height,
		0, color, color);
	
	gui->draw = XftDrawCreate(display->dpy,gui->win,display->visual,
						display->colormap);

	snprintf(buf,BUF_LEN-1,"%s/%s",conf_get(conf,"theme_path"),
			conf_get(conf,"background.image"));
	gui->background = image_load(display,buf, &w, &h);
	if (!gui->background) {
		gui_delete(gui);
		return NULL;
	}

	gui->title = gui_label_new(display, conf_get(conf,"title.font"),
					conf_get(conf,"title.color"),
					atoi(conf_get(conf,"title.x")),
					atoi(conf_get(conf,"title.y")),
					conf_get(conf,"title.caption"));
	if (!gui->title) {
		gui_delete(gui);
		return NULL;
	}

	gui->username = gui_label_new(display, conf_get(conf,"username.font"),
					conf_get(conf,"username.color"),
					atoi(conf_get(conf,"username.x")),
					atoi(conf_get(conf,"username.y")),
					conf_get(conf,"username.caption"));
	if (!gui->username) {
		gui_delete(gui);
		return NULL;
	}

	snprintf(buf,BUF_LEN-1,"%s/%s",conf_get(conf,"theme_path"),
			conf_get(conf,"username_input.image"));
	gui->user_input = gui_input_new(display, buf,
					atoi(conf_get(conf,"username_input.x")),
					atoi(conf_get(conf,"username_input.y")),
					conf_get(conf,"username_input.text.font"),
					conf_get(conf,"username_input.text.color"),
					atoi(conf_get(conf,"username_input.text.x")),
					atoi(conf_get(conf,"username_input.text.y")),
					atoi(conf_get(conf,"username_input.text.width")),
					atoi(conf_get(conf,"username_input.text.height")));

	if (!gui->user_input) {
		gui_delete(gui);
		return NULL;
	}

	/* TODO: load the rest of the graphics.  */

	return gui;
}

void gui_delete(gui_t *gui)
{
	display_t *display = gui->display;
	
	XftDrawDestroy(gui->draw);

	if (gui->background)
		XFreePixmap(display->dpy,gui->background);

	if (gui->title)
		gui_label_delete(gui->title,display);

	if (gui->username)
		gui_label_delete(gui->username,display);
	
	if (gui->password)
		gui_label_delete(gui->password,display);

	if (gui->user_input)
		gui_input_delete(gui->user_input,display);

	if (gui->passwd_input)
		gui_input_delete(gui->passwd_input,display);

	free(gui);
}

void gui_show(gui_t *gui)
{
	display_t *display = gui->display;

	XSelectInput(display->dpy, gui->win, ExposureMask | KeyPressMask);

	XSetWindowBackgroundPixmap(display->dpy, gui->win, gui->background);

	XMapWindow(display->dpy, gui->win);
	XMoveWindow(display->dpy, gui->win, gui->x, gui->y);

	XGrabKeyboard(display->dpy, gui->win, False, GrabModeAsync, GrabModeAsync, CurrentTime);

	XFlush(display->dpy);
}

void gui_events(gui_t *gui, XEvent *event)
{
	switch(event->type) {
	case Expose:
		gui_draw(gui);
		break;
	case KeyPress:
		gui_keypress(gui,event);
		break;
	}
}

