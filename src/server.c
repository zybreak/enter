#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "enter.h"

#include "server.h"

#define SERVER_TIMEOUT 5

static int server_started = FALSE;
static GPid server_pid;

void child_setup(gpointer user_data);

void child_setup(gpointer user_data)
{
	signal(SIGUSR1,SIG_IGN);
}

static void server_callback(int signal)
{
	if (signal == SIGUSR1)
		server_started = TRUE;
}

static int server_timeout(time_t timeout)
{
	time_t start_time = time(NULL);
	pid_t pidfound = waitpid(server_pid, NULL, WNOHANG);

	while (pidfound != server_pid) {
		pidfound = waitpid(server_pid, NULL, WNOHANG);

		if ((time(NULL) - start_time) > timeout)
			return FALSE;

		usleep(100000);
	}

	return TRUE;
}

static int server_startup(time_t timeout)
{
	time_t start_time = time(NULL);

	while (server_started == FALSE) {
		if ((time(NULL) - start_time) > timeout)
			return FALSE;

		usleep(100000);
	}

	return TRUE;
}

gboolean server_stop(void)
{
	if (server_started == FALSE)
		return TRUE;

	/* Send SIGTERM to the server process.  */
	if (killpg(server_pid, SIGTERM) < 0) {
		g_warning("Could not kill server (%d): %s",
						server_pid, strerror(errno));
		return FALSE;
	}

	/* Wait for the process to terminate.  */
	if (!server_timeout(SERVER_TIMEOUT)) {
		/* If the server won't terminate, send SIGKILL.  */
		if (killpg(server_pid, SIGKILL) < 0) {
			g_warning("Could not kill server: %s", strerror(errno));
			return FALSE;
		}
		/* If it STILL won't die, just let it be...  */
		if (!server_timeout(SERVER_TIMEOUT)) {
			g_warning("Could not kill server: %s", strerror(errno));
			return FALSE;
		}
	}

	return TRUE;
}

gboolean server_start(conf_t *conf)
{
	struct sigaction sa;
	
	/* Handle signals.
	 * SIGUSR1 is sent when the X server is ready.  */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = server_callback;
	sigaction(SIGUSR1,&sa,NULL);

	/* TODO: fix cmd */

	char *cmd[] = {
		"Xnest",
		conf_get(conf,"display"),
		NULL, NULL, NULL
	};

	if (strcmp(conf_get(conf, "authentication"), "none") != 0) {
		cmd[2] = "-auth";
		cmd[3] = conf_get(conf,"auth_file");
	}

	GError *error = NULL;
	gboolean spawn_successful = g_spawn_async(
			NULL, cmd, NULL,
			G_SPAWN_SEARCH_PATH,
			child_setup, NULL,
			&server_pid, &error);

	if (!spawn_successful) {
		g_warning("Spawn X failed: %s\n", error->message);
		return FALSE;
	}
	
	if (!server_startup(SERVER_TIMEOUT)) {
		g_warning("Could not start X, server timed out.");
		return FALSE;
	}

	return TRUE;
}

