#ifndef __AUTH_H__
#define __AUTH_H__

#include <X11/Xauth.h>

typedef struct auth_t {
	Xauth auth;
} auth_t;

typedef enum {
	AUTH_MIT_MAGIC_COOKIE,
	AUTH_XDM_AUTHORIZATION
} auth_type;

auth_t* auth_new(auth_type type, const char *hostname, const char *display);
void auth_delete(auth_t *auth);
int auth_write(auth_t *auth, const char *file);

#endif
