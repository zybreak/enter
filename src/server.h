#ifndef SERVER_H_
#define SERVER_H_

#include "conf.h"

gboolean server_stop(void);
gboolean server_start(conf_t *conf);

#endif /*SERVER_H_*/
