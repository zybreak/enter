#include <stdlib.h>

#include "enter.h"

#include "gui.h"
#include "login.h"

static void gui_draw(gui_t *gui)
{
	display_t *display = gui->display;

	//xcb_clear_area(display->dpy, gui->win, 0, 0, gui->width, gui->height);

	/* Draw the widgets.  */
	if (gui->focus) {
		gui_widget_draw_focus(gui->focus, gui);
	}

	void f(gpointer it, gpointer data) {
		gui_t *gui = (gui_t*)data;

		if (it != gui->focus) {
			gui_widget_draw(it, gui);
		}
	}

	g_slist_foreach(gui->widgets, f, gui);

	xcb_flush(display->dpy);
}

static void gui_mappingnotify(gui_t *gui, xcb_mapping_notify_event_t *event)
{
	//XRefreshKeyboardMapping(&event->xmapping);
}

static void gui_keypress(gui_t *gui, xcb_key_press_event_t *event)
{
	//KeySym keysym = XLookupKeysym(&event->xkey,1);
	xcb_keycode_t keycode = event->detail;
	xcb_keysym_t keysym;

	gui->focus->on_key_press(gui->focus, keysym);
}

gui_t* gui_new(display_t *display)
{
	gui_t *gui = g_new(gui_t,1);

	/* Set default options.  */
	gui->x = 0;
	gui->y = 0;
	gui->width = display->screen->width_in_pixels;
	gui->height = display->screen->height_in_pixels;
	gui->display = display;
	gui->widgets = g_slist_alloc();
	gui->focus = NULL;

	uint32_t values[] = {
		XCB_EVENT_MASK_EXPOSURE | 
		XCB_EVENT_MASK_KEY_PRESS |
		XCB_EVENT_MASK_KEY_RELEASE
	};
	/* Create the GUI window.  */
	gui->win = xcb_generate_id(display->dpy);
	xcb_create_window(
			display->dpy,
			display->screen->root_depth,
			gui->win,
			display->screen->root,
			gui->x,
			gui->y,
			gui->width,
			gui->height,
			0,
			XCB_WINDOW_CLASS_INPUT_OUTPUT,
			display->screen->root_visual,
			XCB_CW_EVENT_MASK,
			values);

	gui->drawable = gui->win;

	/* Create a draw surface.  */

	return gui;
}

void gui_delete(gui_t *gui)
{
	display_t *display = gui->display;

	xcb_destroy_window(display->dpy, gui->win);

	void f(gpointer data, gpointer user_data) {
		gui_widget_delete(data);
	}

	g_slist_foreach(gui->widgets, f, NULL);

	g_slist_free(gui->widgets);
	g_free(gui);
}

void gui_show(gui_t *gui)
{
	display_t *display = gui->display;

	xcb_map_window(display->dpy, gui->win);

	uint32_t values[] = {
		gui->x,
		gui->y
	};

	xcb_configure_window(display->dpy, gui->win,
			XCB_CONFIG_WINDOW_X | XCB_CONFIG_WINDOW_Y, values);

	gui_draw(gui);

	xcb_flush(display->dpy);
}

void gui_hide(gui_t *gui)
{
	display_t *display = gui->display;
	xcb_unmap_window(display->dpy, gui->win);
	xcb_flush(display->dpy);
}

void gui_handle_event(gui_t *gui, xcb_generic_event_t *event)
{
	switch(event->response_type & ~0x80) {
		case XCB_EXPOSE:
			gui_draw(gui);
			break;
		case XCB_KEY_PRESS:
			gui_keypress(gui, (xcb_key_press_event_t*)event);
			break;
		case XCB_MAPPING_NOTIFY:
			gui_mappingnotify(gui, (xcb_mapping_notify_event_t*)event);
			break;
	}
	gui_draw(gui);
}

