#include <X11/keysym.h>
#include <stdlib.h>

#include "enter.h"
#include "gui.h"
#include "gui_image.h"

#include "login.h"
#include "log.h"
#include "utils.h"

#define LABEL_NEW(LABEL) \
	gui_label_new(display, conf_get(theme, LABEL ".font"), \
			conf_get(theme, LABEL ".color"), \
			atoi(conf_get(theme, LABEL ".x")), \
			atoi(conf_get(theme, LABEL ".y")), \
			0, 0, \
			conf_get(theme, LABEL ".caption"))

#define INPUT_NEW(INPUT, IMAGE, PASSWORD) \
	gui_input_new(display, buf, \
			atoi(conf_get(theme, INPUT ".x")), \
			atoi(conf_get(theme, INPUT ".y")), \
			conf_get(theme, INPUT ".text.font"), \
			conf_get(theme, INPUT ".text.color"), \
			atoi(conf_get(theme, INPUT ".text.x")), \
			atoi(conf_get(theme, INPUT ".text.y")), \
			atoi(conf_get(theme, INPUT ".text.width")), \
			atoi(conf_get(theme, INPUT ".text.height")), \
			PASSWORD)

#define BUF_LEN 64

static void gui_draw(gui_t *gui)
{
	display_t *display = gui->display;

	/* Only clear if double buffering is not present,
	 * since DBE clears the window for us.  */
	if (!gui->has_doublebuf)
		XClearWindow(display->dpy, gui->win);

	/* Draw the labels.  */
	gui_label_draw(gui->title, gui);
	gui_label_draw(gui->msg, gui);
	
	/* Draw the input boxes.  */
	if (gui->visible == BOTH || gui->visible == USERNAME) {
		gui_label_draw(gui->username, gui);
		gui_input_draw(gui->user_input, gui, (gui->focus == USERNAME));
	}
	
	if (gui->visible == BOTH || gui->visible == PASSWORD) {
		gui_label_draw(gui->password, gui);
		gui_input_draw(gui->passwd_input, gui, (gui->focus == PASSWORD));
	}

	/* Draw the message label.  */
	gui_label_draw(gui->msg, gui);

	if (gui->has_doublebuf) {
		XdbeSwapBuffers(display->dpy, &gui->swap_info, 1);
	} else
		XFlush(display->dpy);
}

static void gui_keypress(gui_t *gui, XEvent *event)
{
	char ch;
	KeySym keysym;
	XComposeStatus cstatus;
	gui_input_t *input;

	XLookupString(&event->xkey, &ch, 1, &keysym, &cstatus);

	/* Assign `input' to the currently focused
	 * input box.  */
	if (gui->focus == USERNAME) {
		input = gui->user_input;
	} else {
		input = gui->passwd_input;
	}

	if (keysym == XK_BackSpace) {
		gui_input_delete_char(input);

	} else if (keysym == XK_Left) {
		gui_input_set_pos(input, -1, INPUT_POS_REL);

	} else if (keysym == XK_Right) {
		gui_input_set_pos(input, 1, INPUT_POS_REL);

	} else if (keysym == XK_Home) {
		gui_input_set_pos(input, 0, INPUT_POS_ABS);

	} else if (keysym == XK_End) {
		gui_input_set_pos(input, LABEL_TEXT_LEN, INPUT_POS_ABS);

	} else if (keysym == XK_Tab && gui->visible == BOTH) {
		/* If both input boxes are visible, change focus.  */
		gui->focus = (gui->focus == USERNAME) ? PASSWORD : USERNAME;

	} else if (keysym == XK_Return) {
		char *usr = gui_input_get_text(gui->user_input);
		char *pwd = gui_input_get_text(gui->passwd_input);

		/* Authenticate user.  */
		int auth = login_authenticate(usr, pwd);

		/* when auth is TRUE, we got a successful login,
		 * EMPTY user, and passwd field and set gui->mode to LOGIN.
		 * if auth is FALSE and gui->focus is USERNAME, just switch focus.
		 * if auth is FALSE and gui->focus is PASSWORD, empty user and passwd
		 * field and scream at the user! */
		if (auth) {
			/* User authenticated successfully.  */
			memset(usr,'\0',strlen(usr));
			memset(pwd,'\0',strlen(pwd));
			gui_input_set_text(gui->user_input, "");
			gui_input_set_text(gui->passwd_input, "");

			gui->mode = LOGIN;

			return;

		} else if (gui->focus == PASSWORD) {
			gui_input_set_text(gui->user_input, "");
			gui_input_set_text(gui->passwd_input, "");
			/* User authentication failed.  */
			gui_label_set_caption(gui->msg,
				"Wrong password or username.");
		}

		/* If gui->visible is either PASSWORD or
		 * USERNAME, switch gui->visible.  */
		if (gui->visible == PASSWORD)
			gui->visible = USERNAME;
		else if (gui->visible == USERNAME)
			gui->visible = PASSWORD;

		/* Change focus.  */
		gui->focus = (gui->focus == USERNAME) ? PASSWORD : USERNAME;
	} else {
		gui_input_insert_char(input, ch);
		gui_label_set_caption(gui->msg,"");
	}

	gui_draw(gui);
}

static void gui_mappingnotify(gui_t *gui, XEvent *event)
{
	XRefreshKeyboardMapping(&event->xmapping);
}

gui_t* gui_new(display_t *display, conf_t *theme)
{
	gui_t *gui = xmalloc(sizeof(*gui));
	char buf[BUF_LEN];

	memset(gui,'\0',sizeof(*gui));

	/* Set default options.  */
	gui->x = 0;
	gui->y = 0;
	gui->width = display->width;
	gui->height = display->height;
	gui->display = display;
	gui->conf = theme;

	unsigned long color = BlackPixel(display->dpy,display->screen);

	/* Create the GUI window.  */
	gui->win = XCreateSimpleWindow(display->dpy, display->root,
		gui->x, gui->y, gui->width, gui->height,
		0, color, color);
	
	/* Check for the DBE extention.  */
	int major, minor;
	gui->has_doublebuf = XdbeQueryExtension(display->dpy, &major, &minor);

	if (gui->has_doublebuf) {
		XdbeSwapInfo swap = {
			.swap_window = gui->win,
			.swap_action = XdbeBackground
		};
		gui->swap_info = swap;

		gui->back_buffer = XdbeAllocateBackBufferName(display->dpy,
			gui->swap_info.swap_window, gui->swap_info.swap_action);
		gui->drawable = gui->back_buffer;
	} else {
		gui->drawable = gui->win;
	}

	/* Create a draw surface.  */
	gui->draw = XftDrawCreate(display->dpy, gui->drawable, display->visual,
					display->colormap);

	
	/* Read the background pixmap.  */
	snprintf(buf,BUF_LEN-1, "%s/%s/%s", THEMEDIR, conf_get(theme, "theme"),
			conf_get(theme, "enter.background"));

	gui_image_t *image = gui_image_new(display, buf, 0, 0);
	if (!image) {
		log_print(LOG_ERR, "Could not load image \"buf\".");
		gui_delete(gui);
		return NULL;
	}
	gui->background = gui_image_pixmap(image);
	gui_image_delete(image);
	
	/* Set the background pixmap.  */
	XSetWindowBackgroundPixmap(display->dpy, gui->win, gui->background);

	/* Load labels.  */
	gui->title = LABEL_NEW("label.title");
	gui->msg = LABEL_NEW("label.msg");
	gui->username = LABEL_NEW("label.username");
	gui->password = LABEL_NEW("label.password");
	
	/* Load input boxes.  */
	snprintf(buf,BUF_LEN-1, "%s/%s/%s", THEMEDIR, conf_get(theme,"theme"), 
			conf_get(theme,"input.username.image"));
	gui->user_input = INPUT_NEW("input.username", buf, 0);
	
	snprintf(buf,BUF_LEN-1, "%s/%s/%s", THEMEDIR, conf_get(theme,"theme"), 
			conf_get(theme,"input.password.image"));
	gui->passwd_input = INPUT_NEW("input.password", buf, 1);

	/* Make sure all object's were loaded.  */
	if (!(gui->title && gui->msg && gui->username && gui->password &&
				gui->passwd_input && gui->user_input)) {
		log_print(LOG_ERR, "Could not load objects.");
		gui_delete(gui);
		return NULL;
	}

	return gui;
}

void gui_delete(gui_t *gui)
{
	display_t *display = gui->display;

	if (gui->has_doublebuf)
		XdbeDeallocateBackBufferName(display->dpy, gui->back_buffer);
	
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

	gui_input_set_text(gui->user_input, "");
	gui_input_set_text(gui->passwd_input, "");

	gui_label_set_caption(gui->msg, "");
	
	display_t *display = gui->display;

	XSelectInput(display->dpy, gui->win, ExposureMask | KeyPressMask);

	XMapWindow(display->dpy, gui->win);
	XMoveWindow(display->dpy, gui->win, gui->x, gui->y);

	XGrabKeyboard(display->dpy, gui->win, False, GrabModeAsync, GrabModeAsync, CurrentTime);

	gui_draw(gui);

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
	gui->mode = LISTEN;
	
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

	return gui->mode;
}

