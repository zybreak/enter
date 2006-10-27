#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <syslog.h>

#include "enter.h"
#include "enter_server.h"
#include "enter_greeter.h"
#include "cfg.h"

#define PIDFILE "/var/run/" PACKAGE ".pid"
#define PIDBUF 20

static void parse_args(int argc, char **argv, cfg_t *conf)
{
	int i;
	
	for (i=1;i<argc;i++) {
		if ((strcmp(argv[i],"-c") == 0) && (i+1 < argc)) {
			conf_set(conf,"config_file",argv[++i]);
		} else if ((strcmp(argv[i],"-d") == 0) && (i+1 < argc)) {
			conf_set(conf,"display",argv[++i]);
		} else if (strcmp(argv[i],"-n") == 0) {
			conf_set(conf,"daemon","false");
		} else if (strcmp(argv[i],"-v") == 0) {
			printf("%s version %s\n",PACKAGE,VERSION);
			exit(EXIT_SUCCESS);
		} else if (strcmp(argv[i],"-h") == 0) {
			printf(
				"usage %s: [options]\n\n"
				"  -c          specify an alternative config file\n"
				"  -d          connect to display\n"
				"  -n          dont run as a daemon\n"
				"  -v          print version information\n"
				"  -h          print avaiable arguments\n"
				"\n",
				argv[0]);
			exit(EXIT_SUCCESS);
		} else {
			printf("unknown argument, try -h");
			exit(EXIT_SUCCESS);
		}
	}
}

static void default_settings(cfg_t *conf)
{
	conf_set(conf,"config_file","/etc/" PACKAGE ".conf");
	conf_set(conf,"daemon","true");
	conf_set(conf,"display",":0");
}

static void daemonize(pid_t *pid, pid_t *sid)
{
	*pid = fork();
	if (*pid < 0) {
		perror("Could not fork process");
		exit(EXIT_FAILURE);
	} else if (*pid > 0) {
		exit(EXIT_SUCCESS);
	}
	
	umask(0);
	
	*sid = setsid();
	
	if (*sid < 0) {
		fprintf(stderr,"could not set sid\n");
		exit(EXIT_FAILURE);
	}
	
	if (chdir("/") < 0) {
		fprintf(stderr,"could not change working directory\n");
		exit(EXIT_FAILURE);
	}
}

void write_pidfile(pid_t pid)
{
	char buf[PIDBUF];
	int length;
	FILE *fp = fopen(PIDFILE,"w");

	if (!fp) {
		syslog(LOG_ERR,"Could not write pidfile: %s.",PIDFILE);
		return;
	}

	memset(buf,'\0',PIDBUF);
	length = snprintf(buf,PIDBUF-1,"%d",pid);
	
	fwrite(buf,sizeof(char),length,fp);
	fclose(fp);

	return;
}

int main(int argc, char **argv)
{
	cfg_t *conf;
	pid_t server_pid, pid, sid;
	
	conf = conf_new();
	
	default_settings(conf);
	
	parse_args(argc,argv,conf);
	
	if (conf_parse(conf,conf_get(conf,"config_file")) == FALSE) {
		exit(EXIT_FAILURE);
	}
	
	if (!strcmp(conf_get(conf,"daemon"),"true")) {
		daemonize(&pid,&sid);
	}

	/*	
	close(STDIN_FILENO);
	close(STDOUT_FILENO);
	close(STDERR_FILENO);
	*/

	openlog(PACKAGE, LOG_NOWAIT, LOG_DAEMON);
	
	write_pidfile(pid);
	
	syslog(LOG_INFO,"Starting X server.");
	server_pid = server_init(conf);
	if (server_pid == FALSE) {
		exit(EXIT_FAILURE);
	}
	
	syslog(LOG_INFO,"Starting greeter application.");
	if (greeter_init(conf) == FALSE) {
		exit(EXIT_FAILURE);
	}
	
	closelog();
	
	return EXIT_SUCCESS;
}

