#ifndef __LOGIN_H__
#define __LOGIN_H__

#include "auth.h"

/**
 * Returns TRUE and saves the user credentials for a later call
 * to auth_login if the specified user could be authenticated.
 * Otherwise returns FALSE.
 */
int login_authenticate(const char *username, const char *password);

/**
 * Switches to the previous authenticated user and starts the user session.
 * Then return TRUE when the session ends. If no previous user was
 * authenticated or a failure occurs, return FALSE.
 */
int login_start_session(const char *display, auth_t *auth, const char *auth_file, const char *login_file);

#endif
