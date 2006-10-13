#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>

#include "config.h"
#include "display.h"
#include "window.h"
#include "cfg.h"

static void parse_args(int argc, char **argv, cfg_t *conf)
{
	int i;
	
	for (i=1;i<argc;i++) {
		if ((strcmp(argv[i],"-t") == 0) && (i+1 < argc)) {
			conf_set(conf,"config_file",argv[++i]);
		} else if ((strcmp(argv[i],"-d") == 0) && (i+1 < argc)) {
			conf_set(conf,"display",argv[++i]);
		} else if (strcmp(argv[i],"-n") == 0) {
			conf_set(conf,"daemon","false");
		} else if (strcmp(argv[i],"-v") == 0) {
			printf("%s version %s\n",PACKAGE,VERSION);
			exit(EXIT_SUCCESS);
		} else if (strcmp(argv[i],"-h") == 0) {
			printf(
				"usage %s: [options]\n\n"
				"  -c          specify an alternative config file\n"
				"  -d          connect to an existing display\n"
				"  -n          dont run as a daemon\n"
				"  -v          print version information\n"
				"  -h          print avaiable arguments\n"
				"\n",
				argv[0]);
			exit(EXIT_SUCCESS);
		} else {
			printf("unknown argument, try -h");
			exit(EXIT_SUCCESS);
		}
	}
}

static void default_settings(cfg_t *conf)
{
	conf_set(conf,"theme","theme_path");
}

int main(int argc, char **argv)
{
	display_t *display;
	window_t *window;
	cfg_t *conf = conf_new();
	
	default_settings(conf);
	
	parse_args(argc, argv, conf);
	
	display = display_init();
	if (!display)
		return EXIT_FAILURE;
	
	display_blank(display);
	
	window = window_init(display,conf);
	if (!window)
		return EXIT_FAILURE;
	
	window_show(window);
	
	while(1);
	
	return EXIT_SUCCESS;	
}
