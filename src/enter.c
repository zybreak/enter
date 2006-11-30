#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

#include "enter.h"
#include "server.h"
#include "greeter.h"
#include "log.h"
#include "conf.h"

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
			"Usage: %s: [OPTIONS]\n\n"
			"  -c          specify an alternative config file\n"
			"  -d          connect to display\n"
			"  -n          dont run as a daemon\n"
			"  -v          print version information\n"
			"  -h          print avaiable arguments\n"
			"\n"
			"Report bugs to <%s>.\n",
			argv[0], PACKAGE_BUGREPORT);
			exit(EXIT_SUCCESS);
		} else {
			printf("unknown argument, try -h\n");
			exit(EXIT_SUCCESS);
		}
	}
}

static void default_settings(cfg_t *conf)
{
	conf_set(conf,"config_file",CONFDIR "/enter.conf");
	conf_set(conf,"daemon","true");
	conf_set(conf,"display",":0");
}

static int daemonize()
{
	pid_t pid, sid;
	pid = fork();
	if (pid < 0) {
		log_print(LOG_EMERG, "could not fork process");
		return FALSE;
	} else if (pid > 0) {
		/* Kill the parent thread.  */
		exit(EXIT_SUCCESS);
	}
	
	umask(0);
	
	sid = setsid();
	
	if (sid < 0) {
		log_print(LOG_EMERG, "could not set sid");
		return FALSE;
	}
	
	if (chdir("/") < 0) {
		log_print(LOG_EMERG, "could not change working directory");
		return FALSE;
	}

	/* Do not use `close' as this causes broken pipe's with the children.  */
	freopen("/dev/null", "r", stdin);
	freopen("/dev/null", "w", stdout);
	freopen("/dev/null", "w", stderr);

	log_daemon(TRUE);

	return TRUE;
}

static void write_pidfile(pid_t pid)
{
	char buf[PIDBUF];
	int length;
	FILE *fp = fopen(PIDFILE,"w");

	if (!fp) {
		log_print(LOG_ERR, "Could not write pidfile: %s.",PIDFILE);
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

int main(int argc, char **argv)
{
	cfg_t *conf;
	pid_t server_pid, greeter_pid;
	
	openlog(PACKAGE, LOG_NOWAIT, LOG_DAEMON);

	if (getuid() != 0) {
		log_print(LOG_EMERG,"Root priviledges needed to run");
		exit(EXIT_FAILURE);
	}
	
	conf = conf_new();
	
	default_settings(conf);

	parse_args(argc, argv, conf);
	
	if (conf_parse(conf,conf_get(conf,"config_file")) == FALSE) {
		log_print(LOG_EMERG, "Could not read config file: \"%s\"",
					conf_get(conf, "config_file"));
		exit(EXIT_FAILURE);
	}
	
	if (!strcmp(conf_get(conf,"daemon"),"true")) {
		if (daemonize() == FALSE) {
			log_print(LOG_EMERG, "Could not daemonize enter.");
			exit(EXIT_FAILURE);
		}
	}
	
	write_pidfile(getpid());

	log_print(LOG_INFO,"Starting X server.");
	server_pid = server_init(conf);
	if (server_pid == FALSE) {
		log_print(LOG_WARNING,"Could not start server");
		exit(EXIT_FAILURE);
	}
	
	while (1) {
		log_print(LOG_INFO,"Starting greeter application.");
		greeter_pid = greeter_init(conf);
		if (greeter_pid == FALSE) {
			log_print(LOG_WARNING,
					"Could not start greeter application");
			exit(EXIT_FAILURE);
		}
		
		pid_t p = waitpid(greeter_pid, NULL, 0);
		if (p == -1) {
			log_print(LOG_WARNING,"Could not wait for greeter.");
			exit(EXIT_FAILURE);
		}
	}


	log_print(LOG_INFO,"Shutting down.");
	
	remove_pidfile();
	conf_delete(conf);
	closelog();
	
	exit(EXIT_SUCCESS);
}

