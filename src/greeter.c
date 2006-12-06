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

struct greeter_t {
	int mode;
	gui_t *gui;
	display_t *display;
	cfg_t *theme;
};

int greeter_run(greeter_t *greeter)
{
	XEvent event;
	
	gui_show(greeter->gui);

	greeter->mode = LISTEN;
	while(greeter->mode == LISTEN) {
		XNextEvent(greeter->display->dpy, &event);
		gui_events(greeter->gui, &event);
		usleep(1);
	}

	gui_hide(greeter->gui);

	switch (greeter->mode) {
	case LOGIN:
		log_print(LOG_INFO, "Logging in user");
		return auth_login();
		break;
	}

	return TRUE;
}

void greeter_mode(greeter_t *greeter, int _mode)
{
	greeter->mode = _mode;
}

greeter_t* greeter_new(cfg_t *conf)
{
	greeter_t *greeter = xmalloc(sizeof(*greeter));

	greeter->mode = LISTEN;

	char *theme_path = conf_get(conf, "theme_path");
	if (!theme_path) {
		log_print(LOG_WARNING, "Could not find any theme.");
		
		free(greeter);
		
		return NULL;
	}

	char *theme_file = xstrcat(theme_path,"/theme");
	greeter->theme = conf_new();

	if (!conf_parse(greeter->theme, theme_file)) {
		log_print(LOG_EMERG,
			"Could not parse theme \"%s\"",
			theme_file);

		free(theme_file);
		conf_delete(greeter->theme);
		free(greeter);

		return NULL;;
	}
	free(theme_file);

	conf_set(greeter->theme, "display",
			conf_get(conf, "display"));
	conf_set(greeter->theme, "theme_path",
			conf_get(conf, "theme_path"));

	greeter->display = display_new(conf);
	if (!greeter->display) {
		log_print(LOG_WARNING, "Could not open display");

		conf_delete(greeter->theme);
		free(greeter);

		return NULL;
	}
	
	greeter->gui = gui_new(greeter, greeter->display, greeter->theme);
	if (!greeter->gui) {
		log_print(LOG_WARNING, "Could not open gui");

		conf_delete(greeter->theme);
		display_delete(greeter->display);
		free(greeter);
		
		return NULL;
	}

	return greeter;
}

void greeter_delete(greeter_t *greeter)
{
	gui_delete(greeter->gui);
	display_delete(greeter->display);
	conf_delete(greeter->theme);
	free(greeter);
}

