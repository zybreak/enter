#ifndef __GREETER_H__
#define __GREETER_H__

#include "auth.h"

void greeter_auth(auth_t *_auth);
int greeter_init(cfg_t *conf);

#endif
