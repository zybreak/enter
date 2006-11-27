#include <X11/Xlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>

#include "enter.h"
#include "greeter.h"
#include "greeter_display.h"
#include "greeter_gui.h"
#include "log.h"
#include "utils.h"

/* This holds the user credidentials.  */
static auth_t *auth = NULL;

/* This is a state variable.  */
static enum {
	WAIT,
	LOGIN
} action = WAIT;

void greeter_auth(auth_t *_auth)
{
	auth = _auth;
	action = LOGIN;
}

static void parse_args(int argc, char **argv, cfg_t *conf)
{
	int i;

	if (argc<=1) {
		fprintf(stderr,"no theme specified, try -h\n");
		exit(EXIT_FAILURE);
	}
	
	for (i=1;i<argc;i++) {
		if (strcmp(argv[i],"-v") == 0) {
			printf("%s version %s\n",PACKAGE,VERSION);
			exit(EXIT_SUCCESS);
		} else if (strcmp(argv[i],"-h") == 0) {
			printf(
				"Usage %s: [OPTIONS] THEME\n\n"
				"  -v          print version information\n"
				"  -h          print avaiable arguments\n"
				"\n"
				"Report bugs to <%s>.\n",
				argv[0], PACKAGE_BUGREPORT);
			exit(EXIT_SUCCESS);
		} else if (i==argc-1) {
			/* This is the last argument,
			 * use it as the theme path.  */
			conf_set(conf,"theme_path",argv[i]);
		} else {
			printf("unknown argument, try -h\n");
			exit(EXIT_FAILURE);
		}
	}
}

static void default_settings(cfg_t *conf)
{
	conf_set(conf,"title.color","#FFFFFF");
	char *display = getenv("DISPLAY");
	if (!display)
		display = ":0";

	conf_set(conf,"display",display);
}

int main(int argc, char **argv)
{
	display_t *display;
	gui_t *gui;
	cfg_t *conf = conf_new();
	XEvent event;

	default_settings(conf);
	
	parse_args(argc, argv, conf);

	openlog(PACKAGE, LOG_NOWAIT, LOG_DAEMON);

	char *theme_path = xstrcat(conf_get(conf,"theme_path"),"/theme");
	if (!conf_parse(conf,theme_path)) {
		log_print(LOG_EMERG,"could not parse config \"%s\"\n",theme_path);
		return EXIT_FAILURE;
	}
	free(theme_path);

	display = display_new(conf);
	if (!display) {
		log_print(LOG_EMERG,"could not open display\n");
		return EXIT_FAILURE;
	}
	
	gui = gui_new(display,conf);
	if (!gui) {
		log_print(LOG_EMERG,"could not open gui\n");
		return EXIT_FAILURE;
	}
	
	gui_show(gui);

	while(action == WAIT) {
		XNextEvent(display->dpy,&event);
		gui_events(gui,&event);
		usleep(1);
	}

	gui_delete(gui);
	conf_delete(conf);
	display_delete(display);
	closelog();

	switch (action) {
	case LOGIN:
		auth_login(auth);
		break;
	}

	/* If we reach here,
	 * we failed for some unexpected reason.  */

	return EXIT_FAILURE;
}
