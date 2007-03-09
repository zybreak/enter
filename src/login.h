#ifndef __GREETER_AUTH_H__
#define __GREETER_AUTH_H__

/**
 * Returns TRUE and saves the user credidentials for a later call
 * to auth_login if the specified user could be authenticated.
 * Otherwize returns FALSE.
 */
int login_authenticate(const char *username, const char *password);

/**
 * Switches to the previous authenticated user and starts the user session.
 * Then return TRUE when the session ends. If no previous user was
 * authenticated or a failure occurs, return FALSE.
 */
int login_start_session(const char *display, const char *auth_file, const char *login_file);

#endif
