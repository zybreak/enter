#include <X11/Xlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

#include "enter.h"
#include "greeter_display.h"
#include "greeter_window.h"
#include "cfg.h"
#include "utils.h"

static void parse_args(int argc, char **argv, cfg_t *conf)
{
	int i;

	if (argc<=1) {
		fprintf(stderr,"no theme specified, try -h\n");
		exit(EXIT_FAILURE);
	}
	
	for (i=1;i<argc;i++) {
		if ((strcmp(argv[i],"-d") == 0) && (i+1 < argc)) {
			conf_set(conf,"display",argv[++i]);
		} else if (strcmp(argv[i],"-v") == 0) {
			printf("%s version %s\n",PACKAGE,VERSION);
			exit(EXIT_SUCCESS);
		} else if (strcmp(argv[i],"-h") == 0) {
			printf(
				"usage %s: [options] THEME\n\n"
				"  -d          connect to display\n"
				"  -v          print version information\n"
				"  -h          print avaiable arguments\n"
				"\n",
				argv[0]);
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
	/* TODO: Should be `getenv("DISPLAY")' */
	conf_set(conf,"display",":0");
}

int main(int argc, char **argv)
{
	display_t *display;
	window_t *window;
	theme_t *theme;
	cfg_t *conf = conf_new();

	default_settings(conf);
	
	parse_args(argc, argv, conf);

	/* Parse theme config file.  */
	char *theme_path = estrcat(conf_get(conf,"theme_path"),"/theme");
	if (!conf_parse(conf,theme_path)) {
		fprintf(stderr,"could not parse config \"%s\"\n",theme_path);
		return EXIT_FAILURE;
	}
	free(theme_path);
	
	display = display_new(conf);
	if (!display) {
		fprintf(stderr,"could not open display\n");
		return EXIT_FAILURE;
	}
	
	theme = theme_new(display,conf);
	if (!theme) {
		fprintf(stderr,"could not load theme\n");
		return EXIT_FAILURE;
	}

	display_background(display,theme);
	
	window = window_new(display,theme);
	if (!window) {
		fprintf(stderr,"could not open window\n");
		return EXIT_FAILURE;
	}
	
	window_show(window);
	
	XEvent event;
	while(1) {
		XNextEvent(display->dpy,&event);
		window_events(window,&event);
		usleep(1);
	}

	display_delete(display);

	return EXIT_SUCCESS;
}
