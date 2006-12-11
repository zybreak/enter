#ifndef __GREETER_AUTH_H__
#define __GREETER_AUTH_H__

#include "conf.h"

/**
 * Returns TRUE and saves the user credidentials for a later call
 * to auth_login if the specified user could be authenticated.
 * Otherwize returns FALSE.
 */
int auth_authenticate(cfg_t *conf, const char *username, const char *password);

/**
 * Switches to the previous authenticated user and starts the user session.
 * Then return TRUE when the session ends. If no previous user was
 * authenticated or a failure occurs, return FALSE.
 */
int auth_login(void);

#endif
