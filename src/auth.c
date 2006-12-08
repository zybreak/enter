#include <pwd.h>
#include <grp.h>
#include <shadow.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "enter.h"
#include "auth.h"
#include "utils.h"
#include "log.h"

static struct auth_t {
	const char *display;
	struct passwd *pwd;
} auth;

int auth_authenticate(cfg_t *conf, const char *username, const char *password)
{
	auth.pwd = getpwnam(username);
	endpwent();
	if (!auth.pwd) {
		return FALSE;
	}

	/* copy the string because conf_delete will free it.  */
	auth.display = strdup(conf_get(conf,"display"));

	struct spwd *shadow = getspnam(auth.pwd->pw_name);
	endspent();
	
	char *correct = (shadow)?shadow->sp_pwdp:auth.pwd->pw_passwd;

	if (!correct)
		return TRUE;

	char *enc = crypt(password, correct);
	if (!strcmp(enc, correct))
		return TRUE;

	return FALSE;
}

static void auth_spawn(void)
{
	/* If theres no shell accociated with the user in
	 * /etc/passwd, assign the user a shell from /etc/shells.  */
	if (!*auth.pwd->pw_shell) {
		setusershell();
		auth.pwd->pw_shell = getusershell();
		endusershell();
	}

	/* Read the group database /etc/group.  */
	if (initgroups(auth.pwd->pw_name, auth.pwd->pw_gid) == -1)
		return;

	if (setgid(auth.pwd->pw_gid) == -1) {
		return;
	}

	if (setuid(auth.pwd->pw_uid) == -1) {
		return;
	}

	chdir(auth.pwd->pw_dir);

	char *args[] = {
		auth.pwd->pw_shell,
		"-login",
		xstrcat(auth.pwd->pw_dir,"/.xinitrc"),
		NULL
	};

	char *env[] = {
		xstrcat("HOME=",auth.pwd->pw_dir),
		xstrcat("SHELL=",auth.pwd->pw_shell),
		xstrcat("USER=",auth.pwd->pw_name),
		xstrcat("LOGNAME=",auth.pwd->pw_name),
		xstrcat("XAUTHORITY=",
				xstrcat(auth.pwd->pw_dir,"/.Xauthority")),
		xstrcat("DISPLAY=",auth.display),
		NULL
	};

	execve(args[0],args,env);
}

int auth_login(void)
{
	pid_t pid = fork();

	if (pid == -1) {
		log_print(LOG_WARNING, "Could not fork process");
		return FALSE;
	} else if (pid == 0) {
		auth_spawn();
		return FALSE;
	}

	pid_t p = waitpid(pid, NULL, 0);
	if (p == -1) {
		log_print(LOG_WARNING,"Could not wait for user session.");
		return FALSE;
	}

	return TRUE;
}

