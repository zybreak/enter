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
			conf_set(conf,"theme_path",argv[i]);
		} else {
			printf("unknown argument, try -h\n");
			exit(EXIT_FAILURE);
		}
	}
}

static void default_settings(cfg_t *conf)
{
	/* Not used at the moment since a theme path is
	 * demanded as an argument.  */
	conf_set(conf,"theme_path","path_to_theme");
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

	window = window_new(display,theme);
	if (!window)
		return EXIT_FAILURE;
	
	window_show(window);
	
	while(1);
	
	return EXIT_SUCCESS;	
}
