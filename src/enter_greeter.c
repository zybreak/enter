#include <sys/types.h>
#include <unistd.h>
#include <syslog.h>

#include "enter.h"
#include "enter_greeter.h"
#include "utils.h"

#define CMD_LEN 256

int greeter_init(cfg_t *conf)
{
	pid_t pid;
	char *cmd[] = {
		conf_get(conf,"greeter_path"),
		conf_get(conf,"theme_path"),
		NULL
	};

	char *env[] = {
		xstrcat("DISPLAY=",conf_get(conf,"display")),
		NULL
	};
	
	pid = fork();
	if (pid == -1) {
		syslog(LOG_WARNING,"Could not fork process");
		return FALSE;
	} else if (pid == 0) {
		execve(cmd[0],cmd,env);
		syslog(LOG_WARNING,"Could not start greeter application");
		return FALSE;
	}
	
	return pid;
}
