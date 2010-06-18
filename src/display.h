#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <xcb/xcb.h>

/**
 * Stores information about an X connection.
 * Also stores information about the current display,
 * such as dimensions and extensions.
 */
typedef struct display_t {
	xcb_screen_t *screen;
	xcb_connection_t *dpy;
} display_t;

/**
 * Creates a new display_t object.
 * @param display_name Specifies which display to connect to.
 * @return A new display_t object
 */
display_t* display_new(const char *display_name);

/**
 * Frees the memory occupied by display.
 * @param display The object to be freed.
*/
void display_delete(display_t *display);

/**
 * Kills all clients, except `window', running on the display.
 * @param display Specifies which display to use.
 * @param window Is the window that wont be killed.
 */
void display_kill_clients(display_t *display, xcb_window_t window);

#endif /*DISPLAY_H_*/
