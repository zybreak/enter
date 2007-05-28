#ifndef GREETER_H_
#define GREETER_H_

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xdbe.h>

#include "display.h"
#include "gui_widget.h"
#include "conf.h"

/**
 * This is the object that handles the high level GUI routines.
 * Which basically means that it is responsible for what should
 * be displayed, when and where.
 */
typedef struct greeter_t {
	gui_t *gui;
	conf_t *theme;
} greeter_t;

/**
 * Create a new greeter_t object.
 * @param display Which display to use.
 * @param theme What theme to use.
 * @return A new greeter_t object.
 */
greeter_t* greeter_new(display_t *display, conf_t *theme);

/**
 * Delete the object allocated by greeter_new.
 * @param greeter The object to free.
 */
void greeter_delete(greeter_t *greeter);

/**
 * Show the greeter window on the display.
 * @param greeter Which greeter_t object to use.
 */
void greeter_show(greeter_t *greeter);

/**
 * Hide the greeter window from the display.
 * @param greeter Which greeter_t object to use.
 */
void greeter_hide(greeter_t *greeter);

/**
 * Event handling routine.
 * @param greeter Which greeter_t object to use.
 * @return A action to be performed.
 */
action_t greeter_run(greeter_t *greeter);

#endif /*GREETER_H_*/
