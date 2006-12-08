#include <X11/keysym.h>
#include <stdlib.h>

#include "enter.h"
#include "gui.h"
#include "gui_image.h"

#include "auth.h"
#include "log.h"
#include "utils.h"

#define LABEL_NEW(LABEL) \
	gui_label_new(display, conf_get(theme, LABEL ".font"), \
			conf_get(theme, LABEL ".color"), \
			atoi(conf_get(theme, LABEL ".x")), \
			atoi(conf_get(theme, LABEL ".y")), \
			0, 0, \
			conf_get(theme, LABEL ".caption"))

#define INPUT_NEW(INPUT, IMAGE) \
	gui_input_new(display, IMAGE, \
			atoi(conf_get(theme, INPUT ".x")), \
			atoi(conf_get(theme, INPUT ".y")), \
			conf_get(theme, INPUT ".text.font"), \
			conf_get(theme, INPUT ".text.color"), \
			atoi(conf_get(theme, INPUT ".text.x")), \
			atoi(conf_get(theme, INPUT ".text.y")), \
			atoi(conf_get(theme, INPUT ".text.width")), \
			atoi(conf_get(theme, INPUT ".text.height")));

#define BUF_LEN 64

static void gui_draw(gui_t *gui)
{
	display_t *display = gui->display;
	
	XClearWindow(display->dpy,gui->win);
	
	gui_label_draw(gui->title, gui);
	gui_label_draw(gui->msg, gui);
	
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
	} else if (keysym == XK_Tab && gui->visible == BOTH) {
		/* If both input boxes are visible, change focus.  */
		if (gui->focus == USERNAME)
			gui->focus = PASSWORD;
		else
			gui->focus = USERNAME;
	} else if (keysym == XK_Return && gui->focus == PASSWORD) {
		char *usr = gui_input_text(gui->user_input);
		char *pwd = gui_input_text(gui->passwd_input);

		/* Authenticate user.  */
		int auth = auth_authenticate(gui->conf, usr, pwd);
			
		memset(usr,'\0',TEXT_LEN);
		memset(pwd,'\0',TEXT_LEN);

		if (auth == TRUE) {
			/* User authenticated successfully.  */
			gui->mode = LOGIN;
			return;
		} else {
			/* User authentication failed.  */
			gui_label_clear(gui->msg, gui);
			gui_label_set_caption(gui->msg,
					"Wrong password or username.");
			gui_label_draw(gui->msg, gui);
		}
		
		if (gui->visible == PASSWORD)
			gui->visible = USERNAME;
		gui->focus = USERNAME;

	} else if (keysym == XK_Return && gui->focus == USERNAME) {
		if (gui->visible == USERNAME)
			gui->visible = PASSWORD;
		gui->focus = PASSWORD;
	} else {
		if (text_len < TEXT_LEN-1) {
			input_text[text_len] = ch;
			input_text[text_len+1] = '\0';
		}
	}

	gui_label_clear(gui->username, gui);
	gui_label_clear(gui->password, gui);
	gui_input_clear(gui->user_input, gui);
	gui_input_clear(gui->passwd_input, gui);
	
	if (gui->visible == BOTH || gui->visible == USERNAME) {
		gui_input_draw(gui->user_input, gui, FALSE);
		gui_label_draw(gui->username, gui);
	}

	if (gui->visible == BOTH || gui->visible == PASSWORD) {
		gui_input_draw(gui->passwd_input, gui, TRUE);
		gui_label_draw(gui->password, gui);
	}

	XFlush(display->dpy);
}

static void gui_mappingnotify(gui_t *gui, XEvent *event)
{
	XRefreshKeyboardMapping(&event->xmapping);
}

gui_t* gui_new(display_t *display, cfg_t *theme)
{
	gui_t *gui = xmalloc(sizeof(*gui));
	char buf[BUF_LEN];

	memset(gui,'\0',sizeof(*gui));

	gui->x = 0;
	gui->y = 0;
	gui->width = display->width;
	gui->height = display->height;
	gui->display = display;
	gui->conf = theme;
	gui->mode = LISTEN;

	unsigned long color = BlackPixel(display->dpy,display->screen);

	gui->win = XCreateSimpleWindow(display->dpy, display->root,
		gui->x, gui->y, gui->width, gui->height,
		0, color, color);
	
	gui->draw = XftDrawCreate(display->dpy,gui->win,display->visual,
						display->colormap);

	snprintf(buf,BUF_LEN-1, "%s/%s", conf_get(theme, "theme_path"),
			conf_get(theme, "enter.background"));

	/* This is an ugly hack, find another way to
	 * get the Pixmap needed to set WindowBackground.  */
	gui_image_t *image = gui_image_new(display, buf, 0, 0);
	if (!image) {
		log_print(LOG_ERR, "could not load image \"buf\"");
		gui_delete(gui);
		return NULL;
	}
	gui->background = gui_image_pixmap(image);
	gui_image_delete(image);
	XSetWindowBackgroundPixmap(display->dpy, gui->win, gui->background);

	gui->title = LABEL_NEW("title");
	gui->msg = LABEL_NEW("msg");
	gui->username = LABEL_NEW("username");
	gui->password = LABEL_NEW("password");

	if (!(gui->title && gui->msg && gui->username && gui->password)) {
		log_print(LOG_ERR, "could not load labels.");
		gui_delete(gui);
		return NULL;
	}

	snprintf(buf,BUF_LEN-1, "%s/%s", conf_get(theme, "theme_path"),
			conf_get(theme, "username_input.image"));

	gui->user_input = INPUT_NEW("username_input", buf);
	if (!gui->user_input) {
		log_print(LOG_ERR, "could not load user_input");
		gui_delete(gui);
		return NULL;
	}

	snprintf(buf,BUF_LEN-1, "%s/%s", conf_get(theme,"theme_path"),
			conf_get(theme,"password_input.image"));

	gui->passwd_input = INPUT_NEW("password_input", buf);
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

	if (gui->msg)
		gui_label_delete(gui->msg,display);

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
	gui->visible = USERNAME;
	gui->focus = USERNAME;

	char *s = conf_get(gui->conf, "enter.visible");
	if (!strcmp(s, "both")) {
		gui->visible = BOTH;
	}

	char *usr = gui_input_text(gui->user_input);
	char *pwd = gui_input_text(gui->passwd_input);

	memset(usr, '\0', TEXT_LEN);
	memset(pwd, '\0', TEXT_LEN);

	gui_label_set_caption(gui->msg, "");
	
	display_t *display = gui->display;

	XSelectInput(display->dpy, gui->win, ExposureMask | KeyPressMask);

	XMapWindow(display->dpy, gui->win);
	XMoveWindow(display->dpy, gui->win, gui->x, gui->y);

	XGrabKeyboard(display->dpy, gui->win, False, GrabModeAsync, GrabModeAsync, CurrentTime);

	XFlush(display->dpy);
}

void gui_hide(gui_t *gui)
{
	display_t *display = gui->display;

	XUnmapWindow(display->dpy, gui->win);
	XUngrabKeyboard(display->dpy, CurrentTime);

	XFlush(display->dpy);
}

int gui_run(gui_t *gui)
{
	XEvent event;
	
	while (gui->mode == LISTEN) {
		XNextEvent(gui->display->dpy, &event);

		switch(event.type) {
		case Expose:
			gui_draw(gui);
			break;
		case KeyPress:
			gui_keypress(gui, &event);
			break;
		case MappingNotify:
			gui_mappingnotify(gui, &event);
			break;
		}
	}

	/* Restore mode to LISTEN.  */
	int mode = gui->mode;
	gui->mode = LISTEN;

	return mode;
}

