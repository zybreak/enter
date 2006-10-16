#include <X11/Xlib.h>
#include <stdlib.h>
#include <stdio.h>

#include "enter.h"
#include "display.h"
#include "window.h"
#include "cfg.h"

static void parse_args(int argc, char **argv, cfg_t *conf)
{
	int i;

	if (argc<=1) {
		fprintf(stderr,"no theme specified\n");
		exit(EXIT_FAILURE);
	}
	
	for (i=1;i<argc-1;i++) {
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
		} else {

			printf("unknown argument, try -h\n");
			exit(EXIT_SUCCESS);
		}
	}
	conf_set(conf,"theme_path",argv[i]);
}

static void default_settings(cfg_t *conf)
{
	conf_set(conf,"theme_path","path_to_theme");
}

int main(int argc, char **argv)
{
	display_t *display;
	window_t *window;
	cfg_t *theme = conf_new();
	
	default_settings(theme);
	
	parse_args(argc, argv, theme);

	/* Parse theme config file.  */
	if (!conf_parse(theme,conf_get(theme,"theme_path")))
		return EXIT_FAILURE;
	
	display = display_init();
	if (!display)
		return EXIT_FAILURE;
	
	window = window_init(display,theme);
	if (!window)
		return EXIT_FAILURE;
	
	window_show(window);
	
	while(1);
	
	return EXIT_SUCCESS;	
}
