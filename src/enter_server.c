#include <X11/Xauth.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <syslog.h>
#include <time.h>

#include "enter.h"
#include "enter_server.h"
#include "utils.h"

#define AUTH_DATA_LEN 16
#define AUTH_NAME "MIT-MAGIC-COOKIE-1"
#define CMD_LEN 256
#define SPAWN_TIMEOUT 5

static int server_started = FALSE;

static int new_cookie(char *cookie)
{
	
	return TRUE;
}

static int server_authenticate(cfg_t *conf, const char *address)
{
	FILE *auth_file;
	Xauth *auth = (Xauth*)xmalloc(sizeof(*auth));
	
	auth->family = FamilyLocal;
	
	/* Copy the address. */
	auth->address_length = strlen(address);
	auth->address = xmalloc(auth->address_length+1);
	memcpy(auth->address,address,auth->address_length);
	
	/* TODO: Copy the display number.  */
	auth->number_length = 0;
	auth->number = 0;
	
	/* Copy the cookie name.  */
	auth->name_length = strlen(AUTH_NAME);
	auth->name = strdup(AUTH_NAME);
	
	/* Generate cookie.  */
	auth->data_length = AUTH_DATA_LEN;
	auth->data = (char*)xmalloc(auth->data_length+1);
	if (new_cookie(auth->data) == FALSE) {
		free(auth->data);
		free(auth->name);
		free(auth);
		return FALSE;
	}
	
	/* Write authentication file.  */
	auth_file = fopen(conf_get(conf,"auth_file"),"w");
	if (!auth_file) {
		syslog(LOG_CRIT, "Failed to write auth file for X Server.");
		return FALSE;
	}
	
	if (!XauWriteAuth(auth_file,auth)) {
		fclose(auth_file);
		syslog(LOG_CRIT, "Failed to write auth file for X Server.");
		return FALSE;
	}
	
	return TRUE;
}

void signal_sigusr1(int signal)
{
	server_started = TRUE;
}

int server_init(cfg_t *conf)
{
	char *cmd[CMD_LEN];
	pid_t server_pid;
	struct sigaction sa;
	time_t start_time;
	
	server_started = FALSE;
	
	/* Handle signals.
	 * SIGURS1 is sent when the X server is ready.  */
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
		syslog(LOG_WARNING,"Could not fork process");
		return FALSE;
		case 0:
		signal(SIGUSR1,SIG_IGN);
		execve(cmd[0],cmd,NULL);
		syslog(LOG_WARNING,"Could not start server");
		return FALSE;
		default:
		start_time = time(NULL);
		
		while (server_started == FALSE) {
			if (time(NULL)-start_time > SPAWN_TIMEOUT)
				break;
			usleep(100000);
		}
		
		if (server_started == FALSE) {
			syslog(LOG_WARNING,"Server timed out");
			/* TODO: Maby make sure the other process is dead here?  */
			return FALSE;
		}
		
		return server_pid;
	}	
}
