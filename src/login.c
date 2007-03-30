#include <pwd.h>
#include <grp.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "enter.h"
#include "login.h"
#include "utils.h"
#include "log.h"

#ifdef HAVE_SHADOW_H
#include <shadow.h>
#endif

static struct passwd *pwd = NULL;

static void auth_spawn(const char *display, auth_t *auth, const char *auth_file, const char *login_file)
{
	/* If theres no shell accociated with the user in
	 * /etc/passwd, assign the user a shell from /etc/shells.  */
	if (!pwd->pw_shell) {
		setusershell();
		pwd->pw_shell = getusershell();
		endusershell();
	}

	/* Read the group database /etc/group.  */
	if (initgroups(pwd->pw_name, pwd->pw_gid) == -1)
		return;

	/* Set the group ID.  */
	if (setgid(pwd->pw_gid) == -1) {
		return;
	}

	/* Set the user ID.  */
	if (setuid(pwd->pw_uid) == -1) {
		return;
	}

	/* Change working directory, to the users home directory.  */
	chdir(pwd->pw_dir);


	/* Write auth file.  */
	if (auth) {
		/* Convert relative to absolute path names.  */
		if (*auth_file != '/') {
			char *new_path = xmalloc(sizeof(*new_path) * 512);
			snprintf(new_path, 511, "%s/%s", pwd->pw_dir, auth_file);
			auth_file = new_path;
		}

		if (unlink(auth_file) == -1) {
			log_print(LOG_EMERG,"Could not remove old auth file.");
		}
		
		if (!auth_write(auth, auth_file)) {
			log_print(LOG_EMERG,"Could not write to auth file.");
			return;
		}
	}

	char *args[] = {
		pwd->pw_shell,
		"-login",
		strdup(login_file),
		NULL
	};

	char *env[] = {
		xstrcat("HOME=", pwd->pw_dir),
		xstrcat("SHELL=", pwd->pw_shell),
		xstrcat("USER=", pwd->pw_name),
		xstrcat("LOGNAME=", pwd->pw_name),
		xstrcat("XAUTHORITY=", auth_file),
		xstrcat("DISPLAY=", display),
		NULL
	};

	execve(args[0],args,env);
}

int login_authenticate(const char *username, const char *password)
{
	char *real_pwd;

	pwd = getpwnam(username);
	endpwent();

	if (!pwd) {
		return FALSE;
	}

	 /* Point real_pwd to the passwd password field.  */
	real_pwd = pwd->pw_passwd;

#ifdef HAVE_SHADOW_H
	/* If we have shadow installed, and the correct
	 * shadow entry is found, point real_pwd to the shadow
	 * passwd field instead.  */
	struct spwd *shadow = getspnam(pwd->pw_name);
	endspent();

	if (shadow) {
		real_pwd = shadow->sp_pwdp;
	}
#endif

	/* If we did not get a password, simply log the user in.  */
	if (!real_pwd)
		return TRUE;

	/* if the passwords match, return TRUE.  */
	char *enc = crypt(password, real_pwd);
	if (!strcmp(enc, real_pwd))
		return TRUE;

	/* else free the user credinentials
	 * and return FALSE.  */
	pwd = NULL;
	
	return FALSE;
}

int login_start_session(const char *display, auth_t *auth, const char *auth_file, const char *login_file)
{
	/* If no previous user was authenticated,
	 * return FALSE. */
	if (!pwd)
		return FALSE;

	/* Fork a new process.  */
	pid_t pid = fork();

	if (pid == -1) {
		log_print(LOG_WARNING, "Could not fork process");
		return FALSE;
	} else if (pid == 0) {
		/* Spawn a user session in the child thread.  */
		auth_spawn(display, auth, auth_file, login_file);
		/* If the user session could not spawn,
		 * reset the auth struct and return false.  */
		
		pwd = NULL;
		
		return FALSE;
	}

	/* Reset the auth struct for subsequent calls.  */
	pwd = NULL;

	/* Wait for the user session in the parent thread.  */
	pid_t p = waitpid(pid, NULL, 0);
	if (p == -1) {
		log_print(LOG_WARNING,"Could not wait for user session.");
		return FALSE;
	}

	return TRUE;
}

