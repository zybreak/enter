#include <X11/keysym.h>
#include <stdlib.h>

#include "enter.h"
#include "greeter.h"

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

#define BUF_LEN 256

static void greeter_keypress(greeter_t *greeter, XEvent *event)
{
#if 0
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
		 * if auth is FALSE and gui->focus is USERNAME, just switch
		 * focus. If auth is FALSE and gui->focus is PASSWORD,
		 * empty user and passwd field and scream at the user! */
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
#endif
}

greeter_t* greeter_new(display_t *display, conf_t *theme)
{
	char buf[BUF_LEN];
	greeter_t *greeter = xmalloc(sizeof(*greeter));

	memset(greeter,'\0',sizeof(*greeter));

	greeter->theme = theme;
	greeter->gui = gui_new(display);
	if (!greeter->gui) {
		free(greeter);
		return NULL;
	}

	/* Read the background pixmap.  */
	snprintf(buf,BUF_LEN-1, "%s/%s/%s", THEMEDIR, conf_get(theme, "theme"),
			conf_get(theme, "enter.background"));

	gui_image_t *image = gui_image_new(display, buf, 0, 0);
	if (!image) {
		log_print(LOG_ERR, "Could not load image \"%s\".", buf);
		greeter_delete(greeter);
		return NULL;
	}

	Pixmap background = gui_image_pixmap(image);
	gui_image_delete(image);
	
	/* Set the background pixmap.  */
	XSetWindowBackgroundPixmap(display->dpy, greeter->gui->win, background);

	/* Load labels.  */
#if 0
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
#endif

	return greeter;
}

void greeter_delete(greeter_t *greeter)
{
	gui_delete(greeter->gui);
	free(greeter);
}

void greeter_show(greeter_t *greeter)
{
	gui_show(greeter->gui);
}

void greeter_hide(greeter_t *greeter)
{
	greeter_hide(greeter);
}

action_t greeter_run(greeter_t *greeter)
{
	XEvent event;
	
	while (1) {
		XNextEvent(greeter->gui->display->dpy, &event);

		switch(event.type) {
		case KeyPress:
			greeter_keypress(greeter, &event);
			break;
		default:
			gui_next_event(greeter->gui);
		}
	}

	return LOGIN;
}

