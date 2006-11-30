#include <X11/Xlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include "enter.h"
#include "greeter.h"
#include "display.h"
#include "gui.h"
#include "log.h"
#include "utils.h"

/* This is a state variable.  */
static int mode = LISTEN;

void greeter_mode(int _mode)
{
	mode = _mode;
}

static int greeter_new(cfg_t *conf)
{
	display_t *display;
	gui_t *gui;
	XEvent event;
	cfg_t *theme = conf_new();

	char *theme_path = xstrcat(conf_get(
				conf,"theme_path"),"/theme");
	if (!conf_parse(theme,theme_path)) {
		log_print(LOG_EMERG,
			"could not parse theme \"%s\"\n",
			theme_path);
		return EXIT_FAILURE;
	}
	free(theme_path);

	conf_set(theme, "display",
			conf_get(conf, "display"));
	conf_set(theme, "theme_path",
			conf_get(conf, "theme_path"));

	display = display_new(conf);
	if (!display) {
		log_print(LOG_EMERG,"could not open display\n");
		return EXIT_FAILURE;
	}
	
	gui = gui_new(display,theme);
	if (!gui) {
		log_print(LOG_EMERG,"could not open gui\n");
		return EXIT_FAILURE;
	}
	
	gui_show(gui);

	while(mode == LISTEN) {
		XNextEvent(display->dpy,&event);
		gui_events(gui,&event);
		usleep(1);
	}

	gui_delete(gui);
	display_delete(display);
	conf_delete(theme);

	return EXIT_FAILURE;
}

int greeter_init(cfg_t *conf)
{
	pid_t pid = fork();
	if (pid == -1) {
		log_print(LOG_WARNING, "Could not fork process");
		return FALSE;
	} else if (pid == 0) {
		greeter_new(conf);
		exit(0);
	}

	return pid;
}

