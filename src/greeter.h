#ifndef __GREETER_H__
#define __GREETER_H__

#include "auth.h"

enum {
	LISTEN,
	LOGIN
};

void greeter_mode(int _mode);
int greeter_init(cfg_t *conf);

#endif
