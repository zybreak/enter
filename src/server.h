#ifndef SERVER_H_
#define SERVER_H_

#include "conf.h"

int server_stop(void);
int server_start(conf_t *conf);

#endif /*SERVER_H_*/
