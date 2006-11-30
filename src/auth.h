#ifndef __GREETER_AUTH_H__
#define __GREETER_AUTH_H__

#include "conf.h"

typedef struct auth_t auth_t;

auth_t* auth_new(cfg_t *conf, const char *username, const char *password);
int auth_login(auth_t *auth);

#endif
