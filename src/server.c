#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>
#include <string.h>

#include "enter.h"

#include "server.h"
#include "log.h"

#define SERVER_TIMEOUT 5

static int server_started = FALSE;
static pid_t server_pid = 0;

void child_setup(gpointer user_data)
{
	signal(SIGUSR1,SIG_IGN);
}

static void server_callback(int signal)
{
	if (signal == SIGUSR1)
		server_started = TRUE;
}

static int server_timeout(pid_t server_pid, time_t timeout)
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

static int server_startup(pid_t server_pid, time_t timeout)
{
	time_t start_time = time(NULL);

	while (server_started == FALSE) {
		if ((time(NULL) - start_time) > timeout)
			return FALSE;

		usleep(100000);
	}

	return TRUE;
}

int server_stop(void)
{
	if (server_started == FALSE)
		return TRUE;

	/* Send SIGTERM to the server process.  */
	if (killpg(server_pid, SIGTERM) < 0) {
		log_print(LOG_ERR, "Could not kill server (%d): %s",
						server_pid, strerror(errno));
		return FALSE;
	}

	/* Wait for the process to terminate.  */
	if (!server_timeout(server_pid, SERVER_TIMEOUT)) {
		/* If the server won't terminate, send SIGKILL.  */
		if (killpg(server_pid, SIGKILL) < 0) {
			log_print(LOG_ERR, "Could not kill server: %s", strerror(errno));
			return FALSE;
		}
		/* If it STILL won't die, just let it be...  */
		if (!server_timeout(server_pid, SERVER_TIMEOUT)) {
			log_print(LOG_ERR, "Could not kill server: %s", strerror(errno));
			return FALSE;
		}
	}

	return TRUE;
}

int server_start(conf_t *conf)
{
	struct sigaction sa;
	
	/* Handle signals.
	 * SIGUSR1 is sent when the X server is ready.  */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = server_callback;
	sigaction(SIGUSR1,&sa,NULL);



	char *cmd[] = {
		"Xnest",
		":1",
		NULL, NULL, NULL
	};

	if (strcmp(conf_get(conf, "authentication"), "none") != 0) {
		cmd[2] = "-auth";
		cmd[3] = conf_get(conf,"auth_file");
	}

	GPid *child_pid;
	GError *error = NULL;
	gboolean spawn_successful = g_spawn_async(
			NULL, cmd, NULL,
			G_SPAWN_SEARCH_PATH,
			child_setup, NULL,
			child_pid, &error);

	if (!spawn_successful) {
		log_print(LOG_ERR, "Spawn X failed: %s\n", error->message);
		return FALSE;
	}
	
	if (!server_startup(server_pid, SERVER_TIMEOUT)) {
		log_print(LOG_ERR, "Could not start X, server timed out.");
		return FALSE;
	}

	return server_pid;
}

