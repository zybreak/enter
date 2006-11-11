#include <X11/Xlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>

#include "enter.h"
#include "greeter.h"
#include "greeter_display.h"
#include "greeter_gui.h"
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

void greeter_authenticate(const char *usr, const char *pwd, cfg_t *conf)
{
	printf("Logging in \"%s\" (%s)\n",usr,
					pwd);

	struct passwd *p = getpwnam(usr);
	endpwent();

	if (!p) {
		/* Oops. read errno  */
		return;
	}

	if (!*p->pw_shell) {
		setusershell();
		p->pw_shell = getusershell();
		endusershell();
	}

	if (chdir(p->pw_dir) < 0) {
		/* Oops. but hey. who cares right.  */
	}

	if (initgroups(p->pw_name,p->pw_gid) != 0) {
		return;
	}

	if (setgid(p->pw_gid) != 0) {
		return;
	}

	if (setuid(p->pw_uid) != 0) {
		return;
	}

	char *args[] = {
		p->pw_shell,
		"-login",
		xstrcat(p->pw_dir,"/.xinitrc"),
		NULL
	};

	char *env[] = {
		xstrcat("HOME=",p->pw_dir),
		xstrcat("SHELL=",p->pw_shell),
		xstrcat("USER=",p->pw_name),
		xstrcat("LOGNAME=",p->pw_name),
		xstrcat("DISPLAY=",conf_get(conf,"display")),
		NULL
	};

	execve(args[0],args,env);
}

int main(int argc, char **argv)
{
	display_t *display;
	gui_t *gui;
	cfg_t *conf = conf_new();
	XEvent event;

	default_settings(conf);
	
	parse_args(argc, argv, conf);

	char *theme_path = xstrcat(conf_get(conf,"theme_path"),"/theme");
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
	
	gui = gui_new(display,conf);
	if (!gui) {
		fprintf(stderr,"could not open gui\n");
		return EXIT_FAILURE;
	}
	
	gui_show(gui);

	while(1) {
		XNextEvent(display->dpy,&event);
		gui_events(gui,&event);
		usleep(1);
	}

	gui_delete(gui);
	conf_delete(conf);
	display_delete(display);

	return EXIT_SUCCESS;
}
