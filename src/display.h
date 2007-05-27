#ifndef DISPLAY_H_
#define DISPLAY_H_

#include <X11/Xlib.h>
#include <X11/extensions/Xdbe.h>

/**
 * Stores information about an X connection.
 * Also stores information about the current display,
 * such as dimensions and extensions.
 */
typedef struct display_t {
	Window root;
	GC gc;
	int screen;
	int width;
	int height;
	int depth;
	int has_doublebuffer;
	Display *dpy;
	Colormap colormap;
	Visual* visual;
} display_t;

/**
 * Creates a new display_t object.
 * @param display_name Specifies which display to connect to.
 * @return A new display_t object
 */
display_t* display_new(const char *display_name);

/**
 * Frees the memory occupied by display.
 * @param display The object to be free'd.
*/
void display_delete(display_t *display);

/**
 * Kills all clients, except `window', running on the display.
 * @param display Specifies which display to use.
 * @param window Is the window that wont be killed.
 */
void display_kill_clients(display_t *display, Window window);

/**
 * Check if the DBE extention is available.
 * @param display Specifies which display to use.
 * @return TRUE if the extension exists, otherwise FALSE.
 */
int display_has_doublebuffer(display_t *display);

#endif /*DISPLAY_H_*/
