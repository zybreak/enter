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

#define AUTH_DATA_LEN 32
#define AUTH_NAME "MIT-MAGIC-COOKIE-1"
#define SERVER_TIMEOUT 5
#define HOSTNAME_LEN 1024

static int server_started = FALSE;
static pid_t server_pid = 0;

static char* generate_cookie(int length)
{
	int i = 0;
	int hexcount = 0;
	const char digits[] = "0123456789abcdef";
	char *cookie = xmalloc(sizeof(char) * length);

	srand( time(NULL) );
	while (i < length -1) {
		cookie[i++] = digits[rand() % 16];
		if (cookie[i] >= 'a')
			hexcount++;
	}
	
	/* MIT-COOKIE: demands even occurrences of digits and hex digits.
	 * So we check if there was a odd number of digits.*/
	if (hexcount % 2) {
		/* generate a character.  */
		cookie[i] = rand() % 5+10;
	} else {
		/* generate a number.  */
		cookie[i] = rand()%10;
	}

	return cookie;
}

static int server_authenticate(conf_t *conf)
{
	FILE *auth_file;
	char hostname[HOSTNAME_LEN];
	Xauth *auth = xmalloc(sizeof(*auth));

	memset(hostname, 0, HOSTNAME_LEN);
	gethostname(hostname, HOSTNAME_LEN-1);

	auth->family = FamilyLocal;
	
	/* Copy the address. */
	auth->address_length = strlen(hostname);
	auth->address = strdup(hostname);

	/* Copy the display number.  */
	/* TODO: move authentication code to own file.
	 * set disp to rindex(DisplayString(),":")  */
	char *disp = "0";
	auth->number = disp;
	auth->number_length = strlen(auth->number);
	
	/* Copy the cookie name.  */
	auth->name_length = strlen(AUTH_NAME);
	auth->name = strdup(AUTH_NAME);
	
	/* Generate cookie.  */
	auth->data_length = AUTH_DATA_LEN;
	auth->data = generate_cookie(auth->data_length);
	if (!auth->data) {
		log_print(LOG_ERR, "Could not generae magic cookie.");
		
		free(auth->name);
		free(auth->address);
		free(auth);
		
		return FALSE;
	}

	if (XauLockAuth(conf_get(conf, "auth_file"), 3, 3, 0) != LOCK_SUCCESS) {
		log_print(LOG_ERR, "Failed to lock magic cookie file.");

		free(auth->name);
		free(auth->address);
		free(auth);

		return FALSE;
	}
	
	/* Write authentication file.  */
	auth_file = fopen(conf_get(conf,"auth_file"),"w");
	if (!auth_file) {
		log_print(LOG_ERR, "Failed to open magic cookie file.");
		
		free(auth->name);
		free(auth->address);
		free(auth);
		
		return FALSE;
	}
	
	if (!XauWriteAuth(auth_file, auth)) {
		log_print(LOG_ERR, "Failed to write magic cookie.");
		
		fclose(auth_file);
		
		return FALSE;
	}
	
	fclose(auth_file);
	XauUnlockAuth(conf_get(conf, "auth_file"));
	
	setenv("XAUTHORITY", conf_get(conf, "auth_file"), 1);

	/* TODO: free auth when not needed.  */
	
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

int server_start(conf_t *conf)
{
	struct sigaction sa;
	time_t start_time;
	
	/* Handle signals.
	 * SIGUSR1 is sent when the X server is ready.  */
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	sa.sa_handler = signal_sigusr1;
	sigaction(SIGUSR1,&sa,NULL);

	int authenticate = !strcmp(conf_get(conf, "authenticate"), "true");

	/* This is the command to start
	 * the X server.  */
	char *cmd[] = {
		conf_get(conf,"server_path"),
		conf_get(conf,"display"),
		NULL, NULL, NULL
	};
	if (authenticate) {
		/* TODO: make sure the auth file is removed, before creating a new one.  */
		if (!server_authenticate(conf)) {
			log_print(LOG_CRIT, "Could not create magic cookie.");
			return FALSE;
		}

		cmd[2] = "-auth";
		cmd[3] = conf_get(conf,"auth_file");
	}
	
	server_pid = fork();
	switch(server_pid) {
		case -1:
			log_print(LOG_CRIT,"Could not fork process");
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

				return FALSE;
			}
	}

	return server_pid;
}

