#include <pwd.h>
#include <grp.h>
#include <shadow.h>
#include <stdlib.h>
#include <unistd.h>
#include <crypt.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "enter.h"
#include "auth.h"
#include "utils.h"
#include "log.h"

static struct auth_t {
	const char *display;
	const char *auth_file;
	const char *login_file;
	struct passwd *pwd;
} auth;

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

	/* Set the group ID.  */
	if (setgid(auth.pwd->pw_gid) == -1) {
		return;
	}

	/* Set the user ID.  */
	if (setuid(auth.pwd->pw_uid) == -1) {
		return;
	}

	/* Change working directory, to the users home directory.  */
	chdir(auth.pwd->pw_dir);

	char *args[] = {
		auth.pwd->pw_shell,
		"-login",
		auth.login_file,
		NULL
	};

	char *env[] = {
		xstrcat("HOME=", auth.pwd->pw_dir),
		xstrcat("SHELL=", auth.pwd->pw_shell),
		xstrcat("USER=", auth.pwd->pw_name),
		xstrcat("LOGNAME=", auth.pwd->pw_name),
		xstrcat("XAUTHORITY=", auth.auth_file),
		xstrcat("DISPLAY=", auth.display),
		NULL
	};

	execve(args[0],args,env);
}

int auth_authenticate(const char *username, const char *password)
{
	auth.pwd = getpwnam(username);
	endpwent();

	if (!auth.pwd) {
		return FALSE;
	}

	struct spwd *shadow = getspnam(auth.pwd->pw_name);
	endspent();

	/* point to the correct password,
	 * if NULL, skip password authentication.  */
	char *correct = (shadow)?shadow->sp_pwdp:auth.pwd->pw_passwd;
	if (!correct)
		return TRUE;

	/* if the passwords match, return TRUE.  */
	char *enc = crypt(password, correct);
	if (!strcmp(enc, correct))
		return TRUE;

	/* else free the user credinentials
	 * and return FALSE.  */
	auth.pwd = NULL;
	
	return FALSE;
}

int auth_login(const char *display, const char *auth_file, const char *login_file)
{
	/* If no previous user was authenticated,
	 * return FALSE. */
	if (!auth.pwd)
		return FALSE;

	auth.display = display;
	auth.auth_file = auth_file;
	auth.login_file = login_file;
	
	/* Fork a new process.  */
	pid_t pid = fork();

	if (pid == -1) {
		log_print(LOG_WARNING, "Could not fork process");
		return FALSE;
	} else if (pid == 0) {
		/* Spawn a user session in the child thread.  */
		auth_spawn();
		/* If the user session could not spawn,
		 * reset the auth struct and return false.  */
		
		auth.pwd = NULL;
		
		return FALSE;
	}

	/* Reset the auth struct for subsequent calls.  */

	auth.pwd = NULL;

	/* Wait for the user session in the parent thread.  */
	pid_t p = waitpid(pid, NULL, 0);
	if (p == -1) {
		log_print(LOG_WARNING,"Could not wait for user session.");
		return FALSE;
	}

	return TRUE;
}

