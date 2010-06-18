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
#include "utils.h"

#define SERVER_TIMEOUT 5

static int server_started = FALSE;
static pid_t server_pid = 0;

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
		conf_get(conf,"server_path"),
		conf_get(conf,"display"),
		NULL, NULL, NULL
	};

	if (strcmp(conf_get(conf, "authentication"), "none") != 0) {
		cmd[2] = "-auth";
		cmd[3] = conf_get(conf,"auth_file");
	}
	
	server_pid = fork();
	switch(server_pid) {
		case -1:
			log_print(LOG_ERR, "Could not fork process.");
			return FALSE;
		case 0:
			signal(SIGUSR1,SIG_IGN);
			execve(cmd[0],cmd,NULL);

			/* If we reach this line, the exec call failed.  */
			return FALSE;
		default:
			if (!server_startup(server_pid, SERVER_TIMEOUT)) {
				log_print(LOG_ERR, "Could not start X, server timed out.");
				return FALSE;
			}
	}

	return server_pid;
}

