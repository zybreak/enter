#ifndef GREETER_H_
#define GREETER_H_

#include <X11/Xlib.h>
#include <X11/Xft/Xft.h>
#include <X11/extensions/Xdbe.h>

#include "display.h"
#include "gui_widget.h"
#include "conf.h"

typedef enum action_t {
	LOGIN
} action_t;

typedef struct greeter_t {
	gui_t *gui;
	conf_t *theme;
} greeter_t;

greeter_t* greeter_new(display_t *display, conf_t *theme);
void greeter_delete(greeter_t *greeter);
void greeter_show(greeter_t *greeter);
void greeter_hide(greeter_t *greeter);
action_t greeter_run(greeter_t *greeter);

#endif /*GREETER_H_*/
