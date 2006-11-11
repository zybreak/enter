#include <stdlib.h>

#include "enter.h"
#include "greeter.h"
#include "greeter_gui.h"
#include "greeter_image.h"
#include "utils.h"

#define BUF_LEN 64

static void gui_draw(gui_t *gui)
{
	display_t *display = gui->display;
	
	XClearWindow(display->dpy,gui->win);
	
	gui_label_draw(gui->title, gui);
	
	if (gui->visible == ALL || gui->visible == USERNAME) {
		gui_label_draw(gui->username, gui);
		gui_input_draw(gui->user_input, gui, FALSE);
	}
	
	if (gui->visible == ALL || gui->visible == PASSWORD) {
		gui_label_draw(gui->password, gui);
		gui_input_draw(gui->passwd_input, gui, TRUE);
	}

	XFlush(display->dpy);
}

static void gui_keypress(gui_t *gui, XEvent *event)
{
	char ch;
	KeySym keysym;
	XComposeStatus cstatus;
	gui_input_t *input;
	int text_len;

	XLookupString(&event->xkey,&ch,1,&keysym,&cstatus);

	if (gui->visible == ALL) {
		if (gui->focus == USERNAME)
			input = gui->user_input;
		else
			input = gui->passwd_input;
	} else if (gui->visible == USERNAME) {
		input = gui->user_input;
	} else {
		input = gui->passwd_input;
	}
	
	text_len = strlen(input->text);

	if (keysym == XK_BackSpace) {
		if (text_len > 0)
			input->text[text_len-1] = '\0';
	} else if (keysym == XK_Tab) {
		if (gui->visible == ALL) {
			if (gui->focus == USERNAME)
				gui->focus = PASSWORD;
			else
				gui->focus = USERNAME;
		}
	} else if (keysym == XK_Return) {
		if (gui->visible == PASSWORD || gui->focus == PASSWORD) {
			/* Authenticate user.  */
			greeter_authenticate(gui->user_input->text,
					gui->passwd_input->text,
					gui->conf);
			/* It failed.  */
			memset(gui->user_input->text,'\0',TEXT_LEN);
			memset(gui->passwd_input->text,'\0',TEXT_LEN);
			if (gui->visible == PASSWORD)
				gui->visible = USERNAME;
			gui->focus = USERNAME;
			perror("could not login");

		} else if (gui->visible == USERNAME) {
			gui->focus = gui->visible = PASSWORD;
		} else if (gui->visible == ALL) {
			if (gui->focus == USERNAME)
				gui->focus = PASSWORD;
		}
	} else {
		if (text_len < TEXT_LEN-1)
			input->text[text_len] = ch;
	}

	gui_draw(gui);
}

gui_t* gui_new(display_t *display, cfg_t *conf)
{
	gui_t *gui = xmalloc(sizeof(*gui));
	char buf[BUF_LEN];
	int w, h;

	memset(gui,'\0',sizeof(*gui));

	gui->visible = USERNAME;
	gui->focus = USERNAME;
	gui->x = 0;
	gui->y = 0;
	gui->width = display->width;
	gui->height = display->height;
	gui->display = display;
	gui->conf = conf;

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

	gui->password = gui_label_new(display, conf_get(conf,"password.font"),
					conf_get(conf,"password.color"),
					atoi(conf_get(conf,"password.x")),
					atoi(conf_get(conf,"password.y")),
					conf_get(conf,"password.caption"));
	if (!gui->password) {
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

	snprintf(buf,BUF_LEN-1,"%s/%s",conf_get(conf,"theme_path"),
			conf_get(conf,"password_input.image"));
	gui->passwd_input = gui_input_new(display, buf,
					atoi(conf_get(conf,"password_input.x")),
					atoi(conf_get(conf,"password_input.y")),
					conf_get(conf,"password_input.text.font"),
					conf_get(conf,"password_input.text.color"),
					atoi(conf_get(conf,"password_input.text.x")),
					atoi(conf_get(conf,"password_input.text.y")),
					atoi(conf_get(conf,"password_input.text.width")),
					atoi(conf_get(conf,"password_input.text.height")));

	if (!gui->passwd_input) {
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

