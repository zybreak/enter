#include <X11/Xauth.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#include "enter.h"
#include "server.h"
#include "log.h"
#include "utils.h"

#define AUTH_DATA_LEN 16
#define AUTH_NAME "MIT-MAGIC-COOKIE-1"
#define CMD_LEN 256
#define SERVER_TIMEOUT 5

static int server_started = FALSE;
static pid_t server_pid = 0;

static int generate_cookie(char *cookie)
{
#if 0
	struct MD5Context ctx;
	time_t time;

	MD5Init(&ctx);

	time = time(NULL);
	MD5Update(&ctx, );
#endif

	return 0;
}

static int server_authenticate(cfg_t *conf, const char *address)
{
	FILE *auth_file;
	Xauth *auth = xmalloc(sizeof(*auth));
	
	auth->family = FamilyLocal;
	
	/* Copy the address. */
	auth->address_length = strlen(address);
	auth->address = xmalloc(auth->address_length+1);
	memcpy(auth->address,address,auth->address_length);

	/* Copy the display number.  */	
	auth->number = conf_get(conf, "display");
	auth->number_length = strlen(auth->number);
	
	/* Copy the cookie name.  */
	auth->name_length = strlen(AUTH_NAME);
	auth->name = strdup(AUTH_NAME);
	
	/* Generate cookie.  */
	auth->data_length = AUTH_DATA_LEN;
	auth->data = xmalloc(auth->data_length+1);
	auth->data_length = generate_cookie(auth->data);
	if (!auth->data) {
		free(auth->name);
		free(auth);
		return FALSE;
	}
	
	/* Write authentication file.  */
	auth_file = fopen(conf_get(conf,"auth_file"),"w");
	if (!auth_file) {
		log_print(LOG_ERR, "No auth file supplied.");
		return FALSE;
	}
	
	if (!XauWriteAuth(auth_file,auth)) {
		fclose(auth_file);
		log_print(LOG_ERR, "Failed to write auth file for X Server.");
		return FALSE;
	}
	
	return TRUE;
}

static void signal_sigusr1(int signal)
{
	server_started = TRUE;
}

int server_stop(void)
{
	if (server_started == FALSE)
		return TRUE;

	if (killpg(server_pid, SIGTERM) < 0) {
		log_print(LOG_WARNING, "Could not kill server (%d): %s",
						server_pid, strerror(errno));
		return FALSE;
	}

	time_t start_time = time(NULL);
	int pidfound = 0;

	while (pidfound != server_pid) {
		pidfound = waitpid(server_pid, NULL, WNOHANG);

		if (time(NULL)-start_time > SERVER_TIMEOUT)
			break;

		usleep(100000);
	}

	if (pidfound != server_pid) {
		if (killpg(server_pid, SIGKILL) < 0) {
			log_print(LOG_WARNING, "Could not kill server: %s", strerror(errno));
			return FALSE;
		}
	}

	while (pidfound != server_pid) {
		pidfound = waitpid(server_pid, NULL, WNOHANG);

		if (time(NULL)-start_time > SERVER_TIMEOUT)
			break;

		usleep(100000);
	}

	if (pidfound != server_pid) {
		log_print(LOG_WARNING, "Could not kill server: %s", strerror(errno));
		return FALSE;
	}

	return TRUE;
}

int server_start(cfg_t *conf)
{
	char *cmd[CMD_LEN];
	struct sigaction sa;
	time_t start_time;
	
	/* Handle signals.
	 * SIGUSR1 is sent when the X server is ready.  */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = signal_sigusr1;
	sigaction(SIGUSR1,&sa,NULL);
	
	if (!strcmp(conf_get(conf,"authenticate"),"true")) {
		/* TODO: generate cookie  */
		cmd[0] = conf_get(conf,"server_path");
		cmd[1] = "-auth";
		cmd[2] = conf_get(conf,"auth_file");
		cmd[3] = conf_get(conf,"display");
		cmd[4] = NULL;
	} else {
		cmd[0] = conf_get(conf,"server_path");
		cmd[1] = conf_get(conf,"display");
		cmd[2] = NULL;
	}
	
	server_pid = fork();
	switch(server_pid) {
		case -1:
			log_print(LOG_WARNING,"Could not fork process");
			return FALSE;
		case 0:
			signal(SIGUSR1,SIG_IGN);
			execve(cmd[0],cmd,NULL);
			/* If we reach this line, the exec call failed.  */
			return FALSE;
		default:
			start_time = time(NULL);
		
			while (server_started == FALSE) {
				if (time(NULL)-start_time > SERVER_TIMEOUT)
					break;
				usleep(100000);
			}
		
			if (server_started == FALSE) {
				log_print(LOG_WARNING,"Server timed out");
				/* TODO: Maybe make sure the other process is
				 * dead here?  */
				return FALSE;
			}

	}

	return server_pid;
}

