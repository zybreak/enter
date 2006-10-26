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
	char theme_file[] = "/theme";

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
				"  -d          connect to an existing display\n"
				"  -v          print version information\n"
				"  -h          print avaiable arguments\n"
				"\n",
				argv[0]);
			exit(EXIT_SUCCESS);
		} else if (i==argc-1) {
			/* This is the last argument,
			 * use it as the theme path.  */
			char *theme_path = estrcat(argv[i],theme_file);
			conf_set(conf,"theme_path",theme_path);
			free(theme_path);
		} else {
			printf("unknown argument, try -h\n");
			exit(EXIT_FAILURE);
		}
	}
}

static void default_settings(cfg_t *conf)
{
	conf_set(conf,"title.color","#FFFFFF");
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
	if (!conf_parse(conf,conf_get(conf,"theme_path")))
		return EXIT_FAILURE;
	
	display = display_init();
	if (!display)
		return EXIT_FAILURE;
	
	theme = theme_new(display,conf);
	if (!theme)
		return EXIT_FAILURE;

	display_background(display,theme);

	window = window_new(display,theme);
	if (!window)
		return EXIT_FAILURE;
	
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