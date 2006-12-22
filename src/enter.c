#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "enter.h"
#include "server.h"
#include "gui.h"
#include "log.h"
#include "conf.h"
#include "utils.h"
#include "auth.h"

#define PIDFILE "/var/run/" PACKAGE ".pid"
#define PIDBUF 20
#define THEME_LEN 512

static void parse_args(int argc, char **argv, cfg_t *conf)
{
	int i;

	for (i=1;i<argc;i++) {
		if ((strcmp(argv[i],"-c") == 0) && (i+1 < argc)) {
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
			"Usage: %s: [OPTIONS]\n\n"
			"  -c CONFIG   use config file CONFIG\n"
			"  -d DISPLAY  connect to display DISPLAY\n"
			"  -n          dont run as a daemon\n"
			"  -v          print version information\n"
			"  -h          print this help info\n"
			"\n"
			"Report bugs to <%s>.\n",
			argv[0], PACKAGE_BUGREPORT);
			exit(EXIT_SUCCESS);
		} else {
			printf("unknown argument, try -h\n");
			exit(EXIT_SUCCESS);
		}
	}
}

static void default_settings(cfg_t *conf)
{
	conf_set(conf,"config_file",CONFDIR "/enter.conf");
	conf_set(conf,"daemon","true");
	conf_set(conf,"display",":0");
}

static int daemonize()
{
	pid_t pid, sid;
	pid = fork();
	if (pid < 0) {
		log_print(LOG_EMERG, "could not fork process");
		return FALSE;
	} else if (pid > 0) {
		/* Kill the parent thread.  */
		exit(EXIT_SUCCESS);
	}
	
	umask(0);
	
	sid = setsid();
	
	if (sid < 0) {
		log_print(LOG_EMERG, "could not set sid");
		return FALSE;
	}
	
	if (chdir("/") < 0) {
		log_print(LOG_EMERG, "could not change working directory");
		return FALSE;
	}

	/* Do not use `close' as this causes broken pipe's with the children.  */
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);

	log_daemon(TRUE);

	return TRUE;
}

static void write_pidfile(pid_t pid)
{
	char buf[PIDBUF];
	int length;
	FILE *fp = fopen(PIDFILE,"w");

	if (!fp) {
		log_print(LOG_ERR, "Could not write pidfile: %s.",PIDFILE);
		return;
	}

	memset(buf,'\0',PIDBUF);
	length = snprintf(buf,PIDBUF-1,"%d",pid);
	
	fwrite(buf,sizeof(char),length,fp);
	fclose(fp);

	return;
}

static void remove_pidfile()
{
	remove(PIDFILE);
}

int main(int argc, char **argv)
{
	cfg_t *conf = conf_new();
	cfg_t *theme = conf_new();
	
	/* Assign default settings to conf
	 * and parse command line arguments.  */
	default_settings(conf);
	parse_args(argc, argv, conf);

	/* Check if we have enough priviledges.  */
	if (getuid() != 0) {
		log_print(LOG_EMERG,"Not enough priviledges to run");
		exit(EXIT_FAILURE);
	}
	
	openlog(PACKAGE, LOG_NOWAIT, LOG_DAEMON);
	
	/* Parse config file.  */
	if (conf_parse(conf, conf_get(conf,"config_file")) == FALSE) {
		log_print(LOG_EMERG, "Could not read config file: \"%s\"",
					conf_get(conf, "config_file"));
		closelog();
		exit(EXIT_FAILURE);
	}

	/* Parse theme file.  */
	char theme_file[THEME_LEN];
	snprintf(theme_file, THEME_LEN-1, "%s/%s/theme", THEMEDIR,
			conf_get(conf, "theme"));

	if (conf_parse(theme, theme_file) == FALSE) {
		log_print(LOG_EMERG,
			"Could not parse theme \"%s\"",
			theme_file);
		closelog();
		exit(EXIT_FAILURE);
	}

	/* Add theme name to theme, so the full path of datafiles
	 * can be read by only supplying theme .  */
	conf_set(theme, "theme",
			conf_get(conf, "theme"));

	/* Fork to background if "daemon" mode is enabled in the config. */
	if (!strcmp(conf_get(conf,"daemon"),"true")) {
		if (daemonize() == FALSE) {
			log_print(LOG_EMERG, "Could not daemonize enter.");
			closelog();
			exit(EXIT_FAILURE);
		}
	}
	
	write_pidfile(getpid());
	atexit(remove_pidfile);

	log_print(LOG_INFO,"Starting X server.");
	pid_t server_pid = server_start(conf);
	if (server_pid == FALSE) {
		log_print(LOG_EMERG,"Could not start server");
		closelog();
		exit(EXIT_FAILURE);
	}

	/* Connect to the X display.  */
	display_t *display = display_new(conf);
	if (!display) {
		log_print(LOG_EMERG, "Could not connect to display");
		server_stop();
		closelog();
		exit(EXIT_FAILURE);
	}

	/* Create a GUI window.  */
	gui_t *gui = gui_new(display, theme);
	if (!gui) {
		log_print(LOG_EMERG, "Could not open GUI");
		server_stop();
		closelog();
		exit(EXIT_FAILURE);
	}

	while (1) {
		gui_show(gui);
		int action = gui_run(gui);
		gui_hide(gui);

		switch (action) {
		case LOGIN:
			log_print(LOG_INFO, "Logging in user");
			if (auth_login(conf_get(conf, "display")) == FALSE) {
				log_print(LOG_EMERG,
						"Could not open user session");
				
				server_stop();
				closelog();
				exit(EXIT_FAILURE);
			}
			break;
		}
		/* TODO: Kill all clients from the user session!  */
	}


	log_print(LOG_INFO,"Shutting down.");
	
	gui_delete(gui);
	conf_delete(conf);
	conf_delete(theme);
	
	server_stop();
	closelog();
	
	exit(EXIT_SUCCESS);
}

