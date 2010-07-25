#include <stdlib.h>
#include "enter.h"
#include "auth.h"

#define AUTH_MIT_LEN 16
#define AUTH_MIT_NAME "MIT-MAGIC-COOKIE-1"

#define AUTH_XDM_LEN 56
#define AUTH_XDM_NAME "XDM-AUTHORIZATION-1"

static char* generate_mit_magic_cookie(int length)
{
	int i = 0;
	int hexcount = 0;
	const char digits[] = "0123456789abcdef";
	char *cookie = g_new(char, length);

	srand(time(NULL));
	while (i < length -1) {
		cookie[i++] = digits[rand() % 16];
		if (cookie[i] >= 'a')
			hexcount++;
	}
	
	/* MIT-COOKIE: demands even occurrences of digits and hex digits.
	 * So we check if there was a odd number of digits.*/
	if (hexcount % 2) {
		/* generate a character.  */
		cookie[i] = rand() % 5+10;
	} else {
		/* generate a number.  */
		cookie[i] = rand()%10;
	}

	return cookie;
}

static char* generate_xdm_authorization(int length)
{
	/* TODO: implement.  */
	return NULL;
}

static int mit_new(auth_t *auth)
{
	/* Set authentication name.  */
	auth->auth.name = AUTH_MIT_NAME;
	auth->auth.name_length = strlen(auth->auth.name);
	
	/* Generate authentication data.  */
	auth->auth.data_length = AUTH_MIT_LEN;
	auth->auth.data = generate_mit_magic_cookie(auth->auth.data_length);

	if (!auth->auth.data)
		return FALSE;

	return TRUE;
}

static int xdm_new(auth_t *auth)
{
	/* Set authentication name.  */
	auth->auth.name = AUTH_XDM_NAME;
	auth->auth.name_length = strlen(auth->auth.name);
	
	/* Generate authentication data.  */
	auth->auth.data_length = AUTH_XDM_LEN;
	auth->auth.data = generate_xdm_authorization(auth->auth.data_length);

	if (!auth->auth.data)
		return FALSE;

	return TRUE;
}

auth_t* auth_new(auth_type type, const char *hostname, const char *display)
{
	auth_t *auth = g_new(auth_t,1);

	auth->auth.family = FamilyLocal;
	
	/* Copy the address. */
	auth->auth.address = strdup(hostname);
	auth->auth.address_length = strlen(auth->auth.address);

	/* Copy the display number.  */
	auth->auth.number = strdup(display);
	auth->auth.number_length = strlen(auth->auth.number);

	int auth_ok;
	if (type == AUTH_XDM_AUTHORIZATION) {
		auth_ok = xdm_new(auth);
	} else {
		auth_ok = mit_new(auth);
	}

	if (auth_ok == FALSE) {
		g_warning("Could not generate magic cookie.");

		auth_delete(auth);
		
		return NULL;
	}

	return auth;
}

void auth_delete(auth_t *auth)
{
	free(auth->auth.address);
	free(auth->auth.number);
	g_free(auth->auth.data);
	g_free(auth);
}

int auth_write(auth_t *auth, const char *file)
{
	FILE *auth_file;

	if (XauLockAuth(file, 3, 3, 0) != LOCK_SUCCESS) {
		g_warning("Failed to lock authorization file.");

		return FALSE;
	}
	
	/* Write authentication file.  */
	auth_file = fopen(file, "w");
	if (!auth_file) {
		g_warning("Failed to open authorization file.");
		
		return FALSE;
	}
	
	if (!XauWriteAuth(auth_file, &auth->auth)) {
		g_warning("Failed to write authorization data.");
		
		fclose(auth_file);
		
		return FALSE;
	}
	
	fclose(auth_file);
	XauUnlockAuth(file);
	
	return TRUE;
}

