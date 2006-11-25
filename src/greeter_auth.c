#include <pwd.h>
#include <grp.h>
#include <shadow.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include "greeter_auth.h"
#include "utils.h"

#include "log.h"

struct auth_t {
	const char *display;
	struct passwd *pwd;
};

auth_t* auth_new(cfg_t *conf, const char *username, const char *password)
{
	auth_t *auth = xmalloc(sizeof(*auth));
	
	auth->pwd = getpwnam(username);
	endpwent();
	if (!auth->pwd) {
		free(auth);
		return NULL;
	}

	auth->display = strdup(conf_get(conf,"display"));

	struct spwd *shadow = getspnam(auth->pwd->pw_name);
	endspent();
	
	char *correct = (shadow)?shadow->sp_pwdp:auth->pwd->pw_passwd;

	if (!correct)
		return auth;

	char *enc = crypt(password, correct);
	if (!strcmp(enc, correct))
		return auth;

	free(auth);
	return NULL;

}

void auth_login(auth_t *auth)
{
	if (!*auth->pwd->pw_shell) {
		setusershell();
		auth->pwd->pw_shell = getusershell();
		endusershell();
	}

	if (initgroups(auth->pwd->pw_name, auth->pwd->pw_gid))
		return;

	if (setgid(auth->pwd->pw_gid)) {
		return;
	}

	if (setuid(auth->pwd->pw_uid)) {
		return;
	}

	chdir(auth->pwd->pw_dir);

	char *args[] = {
		auth->pwd->pw_shell,
		"-login",
		xstrcat(auth->pwd->pw_dir,"/.xinitrc"),
		NULL
	};

	char *env[] = {
		xstrcat("HOME=",auth->pwd->pw_dir),
		xstrcat("SHELL=",auth->pwd->pw_shell),
		xstrcat("USER=",auth->pwd->pw_name),
		xstrcat("LOGNAME=",auth->pwd->pw_name),
		xstrcat("XAUTHORITY=",
				xstrcat(auth->pwd->pw_dir,"/.Xauthority")),
		xstrcat("DISPLAY=",auth->display),
		NULL
	};

	log_print(LOG_INFO, "display: %s", auth->display);

	execve(args[0],args,env);

}

