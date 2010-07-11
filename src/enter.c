#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>

#include "enter.h"

#include "server.h"
#include "log.h"
#include "conf.h"
#include "login.h"
#include "auth.h"

#define PIDFILE "/var/run/" PACKAGE ".pid"
#define PIDBUF 20
#define THEME_LEN 512

gchar *config_filename = CONFDIR "/enter.conf";
gchar *auth_filename = "/tmp/enter.xauth";
gchar *login_filename = ".xinitrc";
gchar *theme_name = "default";
gboolean no_daemon = FALSE;

static void parse_args(int argc, char **argv)
{
	gboolean version = FALSE;

	GOptionEntry entries[] = {
		{ "config", 'c', 0, G_OPTION_ARG_FILENAME, &config_filename, "Use config file CONFIG", "CONFIG" },
		{ "auth", 'a', 0, G_OPTION_ARG_FILENAME, &auth_filename, "Write server authentication to AUTH", "AUTH" },
		{ "login", 'l', 0, G_OPTION_ARG_FILENAME, &login_filename, "Run FILE instead of ~/.xinitrc", "FILE" },
		{ "theme", 't', 0, G_OPTION_ARG_STRING, &theme_name, "Use theme THEME", "THEME" },
		{ "no-daemon", 'n', 0, G_OPTION_ARG_NONE, &no_daemon, "Don't run as a daemon", NULL },
		{ "version", 'v', 0, G_OPTION_ARG_NONE, &version, "Print version information", NULL },
		NULL
	};

	GError *error = NULL;
	gchar *description = "";
	GOptionContext *context;

	context = g_option_context_new("");
	g_option_context_add_main_entries(context, entries, NULL);
	g_option_context_add_group(context, gtk_get_option_group(FALSE));
	g_option_context_set_help_enabled(context, TRUE);
	g_option_context_set_description(context, description);

	if (!g_option_context_parse(context, &argc, &argv, &error)) {
		g_print("Option parsing failed: %s\n", error->message);
		exit(EXIT_SUCCESS);
	}

	if (version) {
		printf("%s version %s\n", PACKAGE, VERSION);
		exit(EXIT_SUCCESS);
	}
}

static int daemonize()
{
	pid_t pid, sid;
	pid = fork();
	if (pid < 0) {
		log_print(LOG_ERR, "Could not fork to background.");
		return FALSE;
	} else if (pid > 0) {
		/* Kill the parent thread.  */
		exit(EXIT_SUCCESS);
	}
	
	sid = setsid();
	
	if (sid < 0) {
		log_print(LOG_ERR, "Could not set session id.");
		return FALSE;
	}
	
	if (chdir("/") < 0) {
		log_print(LOG_ERR, "Could not change working directory.");
		return FALSE;
	}

	/* Do not use `close' as this causes broken pipe's with the children.  */
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);

	log_daemon(TRUE);

	return TRUE;
}

static auth_t* setup_authentication(conf_t *conf)
{
	auth_t *auth = NULL;
	char hostname[128];
	char *display = rindex(conf_get(conf, "display"), ':');

	/* If the colon wasn't found, set the display to a default value,
	 * otherwise increase display to point beyond the colon.  */
	if (display) {
		display++;
	} else {
		display = "0";
	}

	memset(hostname, '\0', 128);
	gethostname(hostname, 127);

	char *authentication = conf_get(conf, "authentication");

	if (!strcmp(authentication, "magic-cookie")) {
		auth = auth_new(AUTH_MIT_MAGIC_COOKIE, hostname, display);
	} else if (!strcmp(authentication, "xdm")) {
		log_print(LOG_ERR, "XDM is currently not implemented.");
		return NULL;
	} else {
		log_print(LOG_ERR, "'%s' is a unknown authentication protocol.", 
				authentication);
		return NULL;
	}
		
	if (!auth) {
		return NULL;
	}

	if (unlink(conf_get(conf, "auth_file")) == -1) {
		char* error = strerror(errno);
		log_print(LOG_WARNING, "Could not remove auth file: %s.",
				error);
	}

	if (!auth_write(auth, conf_get(conf, "auth_file"))) {
		log_print(LOG_ERR, "Could not write to auth file.");
		return NULL;
	}
	
	setenv("XAUTHORITY", conf_get(conf, "auth_file"), 1);

	return auth;
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

static void shutdown()
{
	remove_pidfile();
	/* TODO: remove auth file.  */
}

int main(int argc, char **argv)
{
	conf_t *conf = conf_new();
	conf_t *theme = conf_new();
	auth_t *auth = NULL;
	
	parse_args(argc, argv);

	/* Check if we have enough privileges.  */
	if (getuid() != 0) {
		log_print(LOG_EMERG, "Not enough privileges to run.");
		exit(EXIT_FAILURE);
	}

	/* If the auth file is going to be shared,
	 * umask needs to be set so everyone has reading
	 * permissions on the files written. Otherwise umask
	 * should be set so only the creator can read and write.  */
	umask(077);
	
	openlog(PACKAGE, LOG_NOWAIT, LOG_DAEMON);
	
	/* Parse config file.  */
	if (conf_parse(conf, config_filename) == FALSE) {
		log_print(LOG_EMERG, "Could not read config file: \"%s\"",
					config_filename);
		closelog();
		exit(EXIT_FAILURE);
	}

	/* Parse theme file.  */
	char theme_file[THEME_LEN];
	snprintf(theme_file, THEME_LEN-1, "%s/%s/theme", THEMEDIR, theme_name);

	if (conf_parse(theme, theme_file) == FALSE) {
		log_print(LOG_EMERG,
			"Could not parse theme \"%s\"",
			theme_file);
		closelog();
		exit(EXIT_FAILURE);
	}

	/* Add theme name to theme, so the full path of data files
	 * can be read by only supplying theme.  */
	conf_set(theme, "theme", theme_name);

	/* Fork to background if "daemon" mode is enabled in the config. */
	if (!no_daemon) {
		if (daemonize() == FALSE) {
			log_print(LOG_EMERG, "Could not daemonize enter.");
			closelog();
			exit(EXIT_FAILURE);
		}
	}
	
	write_pidfile(getpid());
	atexit(shutdown);

	if (strcmp(conf_get(conf, "authentication"), "none") != 0) {
		auth = setup_authentication(conf);
		if (!auth) {
			log_print(LOG_ERR, "Could not setup authentication system, disabling authentication.");
			conf_set(conf, "authentication", "none");
		}
	}

	log_print(LOG_DEBUG, "Starting X server.");
	pid_t server_pid = server_start(conf);
	if (server_pid == FALSE) {
		log_print(LOG_EMERG, "Could not start X server");
		closelog();
		exit(EXIT_FAILURE);
	}

	/* Connect to the X display.  */
	GdkDisplay *display = gdk_display_open(NULL);
	if (!display) {
		log_print(LOG_EMERG, "Could not connect to X display.");
		server_stop();
		closelog();
		exit(EXIT_FAILURE);
	}

	/* Create a GUI window.  */
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

	gtk_widget_show(window);

	gtk_main();

	log_print(LOG_DEBUG, "Shutting down.");
	
	conf_delete(conf);
	conf_delete(theme);
	
	server_stop();
	closelog();
	
	exit(EXIT_SUCCESS);
}

