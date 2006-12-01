#ifndef __GREETER_AUTH_H__
#define __GREETER_AUTH_H__

#include "conf.h"

int auth_authenticate(cfg_t *conf, const char *username, const char *password);
void auth_login(void);

#endif
