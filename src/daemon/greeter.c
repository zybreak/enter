#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>

#include "cfg.h"
#include "config.h"

#define CMD_LEN 256

int greeter_init(cfg_t *conf)
{
	pid_t pid;
	char *cmd[] = {
		conf_get(conf,"greeter_path"),
		NULL
	};
	
	pid = fork();
	if (pid == -1) {
		syslog(LOG_WARNING,"Could not fork process");
		return FALSE;
	} else if (pid == 0) {
		execve(cmd[0],cmd,NULL);
		syslog(LOG_WARNING,"Could not start greeter application");
		return FALSE;
	}
	
	return TRUE;
}
