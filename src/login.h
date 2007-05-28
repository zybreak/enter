#ifndef __LOGIN_H__
#define __LOGIN_H__

#include "auth.h"

/**
 * This object holds a user identification
 * after a successful login.
 */
typedef struct passwd login_t;

/**
 * Authenticates a username with a password.
 * @param username Which username to use.
 * @param password Which password to use.
 * @return A login_t object if the login where successful otherwise NULL.
 */
login_t* login_authenticate(const char *username, const char *password);

/**
 * Starts a user session based upon the provided user identification.
 * @param login The user who wants to login.
 * @param display The display to start the session on.
 * @param auth The X authentication needed.
 * @param auth_file The file to write the X authentication to.
 * @param login_file The file to run as the user session.
 * @return TRUE for a successful session, otherwise FALSE.
 */
int login_start_session(login_t *login, const char *display, auth_t *auth,
			const char *auth_file, const char *login_file);

#endif
