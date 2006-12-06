#ifndef __GREETER_H__
#define __GREETER_H__

#include "auth.h"

enum {
	LISTEN,
	LOGIN
};

typedef struct greeter_t greeter_t;

int greeter_run(greeter_t *greeter);
void greeter_mode(greeter_t *greeter, int _mode);
greeter_t* greeter_new(cfg_t *conf);
void greeter_delete(greeter_t *greeter);

#endif
