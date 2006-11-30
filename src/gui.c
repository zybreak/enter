#include <stdlib.h>

#include "enter.h"
#include "greeter.h"
#include "gui.h"
#include "gui_image.h"
#include "log.h"
#include "utils.h"

#define BUF_LEN 64

static void gui_draw(gui_t *gui)
{
	display_t *display = gui->display;
	
	XClearWindow(display->dpy,gui->win);
	
	gui_label_draw(gui->title, gui);
	
	if (gui->visible == BOTH || gui->visible == USERNAME) {
		gui_label_draw(gui->username, gui);
		gui_input_draw(gui->user_input, gui, FALSE);
	}
	
	if (gui->visible == BOTH || gui->visible == PASSWORD) {
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
	display_t *display = gui->display;

	XLookupString(&event->xkey,&ch,1,&keysym,&cstatus);

	/* Assign `input' to the currently focused
	 * input box.  */
	if (gui->focus == USERNAME) {
		input = gui->user_input;
	} else {
		input = gui->passwd_input;
	}
	
	char *input_text = gui_input_text(input);
	int text_len = strlen(input_text);

	if (keysym == XK_BackSpace) {
		if (text_len > 0)
			input_text[text_len-1] = '\0';
	} else if (keysym == XK_Tab) {
		/* If both input boxes are visible, change focus.  */
		if (gui->visible == BOTH) {
			if (gui->focus == USERNAME)
				gui->focus = PASSWORD;
			else
				gui->focus = USERNAME;
		}
	} else if (keysym == XK_Return) {
		if (gui->focus == PASSWORD) {
			/* Authenticate user.  */
			char *usr = gui_input_text(gui->user_input);
			char *pwd = gui_input_text(gui->passwd_input);
			auth_t *auth = auth_new(gui->conf, usr, pwd);
			
			memset(usr,'\0',TEXT_LEN);
			memset(pwd,'\0',TEXT_LEN);

			/* User authenticated successfully.  */
			if (auth) {
				greeter_auth(auth);
				return;
			}

			if (gui->visible == PASSWORD)
				gui->visible = USERNAME;
			gui->focus = USERNAME;

		} else if (gui->focus == USERNAME) {
			if (gui->visible == USERNAME)
				gui->visible = PASSWORD;
			gui->focus = PASSWORD;
		}
	} else {
		if (text_len < TEXT_LEN-1) {
			input_text[text_len] = ch;
			input_text[text_len+1] = '\0';
		}
	}

	XClearArea(display->dpy, gui->win, gui_input_x(gui->user_input),
			gui_input_y(gui->user_input),
			gui_input_width(gui->user_input),
			gui_input_height(gui->user_input), False);
	gui_input_draw(gui->user_input, gui, FALSE);
	
	XClearArea(display->dpy, gui->win, gui_input_x(gui->passwd_input),
			gui_input_y(gui->passwd_input),
			gui_input_width(gui->passwd_input),
			gui_input_height(gui->passwd_input), False);
	gui_input_draw(gui->passwd_input, gui, TRUE);

	XFlush(display->dpy);
}

static void gui_mappingnotify(gui_t *gui, XEvent *event)
{
	XRefreshKeyboardMapping(&event->xmapping);
}

gui_t* gui_new(display_t *display, cfg_t *conf)
{
	gui_t *gui = xmalloc(sizeof(*gui));
	char buf[BUF_LEN];

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

	char *s = conf_get(conf, "enter.visible");
	if (!strcmp(s, "both")) {
		gui->visible = BOTH;
	}

	snprintf(buf,BUF_LEN-1,"%s/%s",conf_get(conf,"theme_path"),
			conf_get(conf,"enter.background"));

	gui_image_t *image = gui_image_load(display, buf);
	if (!image) {
		log_print(LOG_ERR, "could not load image \"buf\"");
		gui_delete(gui);
		return NULL;
	}
	gui->background = gui_image_pixmap(image);
	gui_image_delete(image);

	gui->title = gui_label_new(display, conf_get(conf,"title.font"),
					conf_get(conf,"title.color"),
					atoi(conf_get(conf,"title.x")),
					atoi(conf_get(conf,"title.y")),
					0, 0,
					conf_get(conf,"title.caption"));
	if (!gui->title) {
		log_print(LOG_ERR, "could not load title");
		gui_delete(gui);
		return NULL;
	}

	gui->username = gui_label_new(display, conf_get(conf,"username.font"),
					conf_get(conf,"username.color"),
					atoi(conf_get(conf,"username.x")),
					atoi(conf_get(conf,"username.y")),
					0, 0,
					conf_get(conf,"username.caption"));
	if (!gui->username) {
		log_print(LOG_ERR, "could not load username");
		gui_delete(gui);
		return NULL;
	}

	gui->password = gui_label_new(display, conf_get(conf,"password.font"),
					conf_get(conf,"password.color"),
					atoi(conf_get(conf,"password.x")),
					atoi(conf_get(conf,"password.y")),
					0, 0,
					conf_get(conf,"password.caption"));
	if (!gui->password) {
		log_print(LOG_ERR, "could not load password");
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
		log_print(LOG_ERR, "could not load user_input");
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
		log_print(LOG_ERR, "could not load passwd_input");
		gui_delete(gui);
		return NULL;
	}

	return gui;
}

void gui_delete(gui_t *gui)
{
	display_t *display = gui->display;
	
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

	XftDrawDestroy(gui->draw);
	XDestroyWindow(display->dpy, gui->win);
	
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
	case MappingNotify:
		gui_mappingnotify(gui, event);
		break;
	}
}

