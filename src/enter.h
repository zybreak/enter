#ifndef ENTER_H_
#define ENTER_H_

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#define FALSE 0
#define TRUE (!FALSE)

/**
 * This enum specifies a number of actions
 * that can be returned by a widget after it reccieves an event.
 */
typedef enum action_t {
	UNHANDLED,
	HANDLED,
	GRAB_FOCUS,
	LOGIN,
	HIBERNATE,
	SHUTDOWN,
	REBOOT
} action_t;

#endif /*ENTER_H_*/
