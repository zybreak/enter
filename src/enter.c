#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "enter.h"

#include "server.h"
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
gchar *display_string = NULL;
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
		{ "display", 'd', 0, G_OPTION_ARG_STRING, &display_string, "Display to connect to", "DISPLAY" },
		{ "version", 'v', 0, G_OPTION_ARG_NONE, &version, "Print version information", NULL },
		{ NULL }
	};

	GError *error = NULL;
	gchar *description = "";
	GOptionContext *context;

	context = g_option_context_new("");
	g_option_context_add_main_entries(context, entries, NULL);
	//g_option_context_add_group(context, gtk_get_option_group(FALSE));
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
		g_warning("Could not fork to background.");
		return FALSE;
	} else if (pid > 0) {
		/* Kill the parent thread.  */
		exit(EXIT_SUCCESS);
	}
	
	sid = setsid();
	
	if (sid < 0) {
		g_warning("Could not set session id.");
		return FALSE;
	}
	
	if (chdir("/") < 0) {
		g_warning("Could not change working directory.");
		return FALSE;
	}

	/* Do not use `close' as this causes broken pipe's with the children.  */
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);

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
		g_warning("XDM is currently not implemented.");
		return NULL;
	} else {
		g_warning("'%s' is a unknown authentication protocol.", 
				authentication);
		return NULL;
	}
		
	if (!auth) {
		return NULL;
	}

	if (unlink(conf_get(conf, "auth_file")) == -1) {
		char* error = strerror(errno);
		g_warning("Could not remove auth file: %s.",
				error);
	}

	if (!auth_write(auth, conf_get(conf, "auth_file"))) {
		g_warning("Could not write to auth file.");
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
		g_warning("Could not write pidfile: %s.",PIDFILE);
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
		g_warning("Not enough privileges to run.");
		exit(EXIT_FAILURE);
	}

	/* If the auth file is going to be shared,
	 * umask needs to be set so everyone has reading
	 * permissions on the files written. Otherwise umask
	 * should be set so only the creator can read and write.  */
	umask(077);
	
	/* Parse config file.  */
	if (conf_parse(conf, config_filename) == FALSE) {
		g_warning("Could not read config file: \"%s\"",
					config_filename);
		exit(EXIT_FAILURE);
	}

	/* Parse theme file.  */
	char theme_file[THEME_LEN];
	snprintf(theme_file, THEME_LEN-1, "%s/%s/theme", THEMEDIR, theme_name);

	if (conf_parse(theme, theme_file) == FALSE) {
		g_warning("Could not parse theme \"%s\"", theme_file);
		exit(EXIT_FAILURE);
	}

	/* Add theme name to theme, so the full path of data files
	 * can be read by only supplying theme.  */
	conf_set(theme, "theme", theme_name);

	/* Set display in config */
	display_string = (display_string) ? display_string : getenv("DISPLAY");
	conf_set(conf, "display", display_string);

	/* Fork to background if "daemon" mode is enabled in the config. */
	if (!no_daemon) {
		if (daemonize() == FALSE) {
			g_warning("Could not daemonize enter.");
			exit(EXIT_FAILURE);
		}
	}
	
	write_pidfile(getpid());
	atexit(shutdown);

	if (strcmp(conf_get(conf, "authentication"), "none") != 0) {
		auth = setup_authentication(conf);
		if (!auth) {
			g_warning("Could not setup authentication system, disabling authentication.");
			conf_set(conf, "authentication", "none");
		}
	}

	g_message("Starting X server.");
	gboolean server_started = server_start(conf);
	if (!server_started) {
		g_warning("Could not start X server");
		exit(EXIT_FAILURE);
	}

	/* Connect to the X display.  */
	GdkDisplay *display = gdk_display_open(conf_get(conf,"display"));
	if (!display) {
		g_warning("Could not connect to X display.");
		server_stop();
		exit(EXIT_FAILURE);
	}

	/* Create a GUI window.  */
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_decorated(GTK_WINDOW(window), FALSE);

	gtk_widget_show(window);

	gtk_main();

	g_message("Shutting down.");
	
	conf_delete(conf);
	conf_delete(theme);
	
	server_stop();
	
	exit(EXIT_SUCCESS);
}

