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
#include "log.h"

#ifdef HAVE_SHADOW_H
#include <shadow.h>
#endif

static void auth_spawn(login_t *pwd, const char *display, auth_t *auth,
	const char *auth_file, const char *login_file)
{
	/* If there's no shell associated with the user in
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
			char *new_path = g_newa(char,512);
			snprintf(new_path, 511, "%s/%s", pwd->pw_dir, auth_file);
			auth_file = new_path;
		}

		if (unlink(auth_file) == -1) {
			log_print(LOG_WARNING, "Could not remove old auth file.");
		}
		
		if (!auth_write(auth, auth_file)) {
			log_print(LOG_ERR, "Could not write to auth file.");
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
		g_strconcat("HOME=", pwd->pw_dir, NULL),
		g_strconcat("SHELL=", pwd->pw_shell, NULL),
		g_strconcat("USER=", pwd->pw_name, NULL),
		g_strconcat("LOGNAME=", pwd->pw_name, NULL),
		g_strconcat("XAUTHORITY=", auth_file, NULL),
		g_strconcat("DISPLAY=", display, NULL),
		NULL
	};

	execve(args[0],args,env);
}

login_t* login_authenticate(const char *username, const char *password)
{
	char *real_pwd;

	login_t* pwd = getpwnam(username);
	endpwent();

	if (!pwd) {
		return NULL;
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
	if (!real_pwd) {
		return pwd;
	}

	/* if the passwords match, return TRUE.  */
	char *enc = crypt(password, real_pwd);
	if (!strcmp(enc, real_pwd))
		return pwd;

	/* else free the user credentials
	 * and return FALSE.  */
	free(pwd);
	
	return NULL;
}

int login_start_session(login_t *login, const char *display, auth_t *auth,
			const char *auth_file, const char *login_file)
{
	/* If no previous user was authenticated,
	 * return FALSE. */
	if (!login)
		return FALSE;

	/* Fork a new process.  */
	pid_t pid = fork();

	if (pid == -1) {
		log_print(LOG_ERR, "Could not fork process.");
		return FALSE;
	} else if (pid == 0) {
		/* Spawn a user session in the child thread.  */

		auth_spawn(login, display, auth, auth_file, login_file);

		/* If the user session could not spawn return FALSE.  */
		
		g_free(login);
		
		return FALSE;
	}

	/* Reset the auth struct for subsequent calls.  */
	g_free(login);

	/* Wait for the user session in the parent thread.  */
	pid_t p = waitpid(pid, NULL, 0);
	if (p == -1) {
		log_print(LOG_ERR, "Could not wait for user session.");
		return FALSE;
	}

	return TRUE;
}

