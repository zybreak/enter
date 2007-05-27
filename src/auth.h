#ifndef __AUTH_H__
#define __AUTH_H__

#include <X11/Xauth.h>

/**
 * Holds X authentication information.
 */
typedef struct auth_t {
	Xauth auth;
} auth_t;

/**
 * Specifies the type of authentication.
 */
typedef enum {
	AUTH_MIT_MAGIC_COOKIE,
	AUTH_XDM_AUTHORIZATION
} auth_type;

/**
 * Creates a new auth_t object.
 * @param type Specifies which type of authentication to use.
 * @param hostname Which hostname should the authentication be associated with.
 * @param display Which display should we use.
 * @return Authentication data.
 */
auth_t* auth_new(auth_type type, const char *hostname, const char *display);

/**
 * Deletes the object allocated by auth_new.
 * @param auth The object to free.
 */
void auth_delete(auth_t *auth);

/**
 * Writes authentication data to file.
 * @param auth Provides authentication data.
 * @param file Write data to file.
 * @return TRUE if successful, otherwise FALSE.
 */
int auth_write(auth_t *auth, const char *file);

#endif
