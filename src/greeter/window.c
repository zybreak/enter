#include <stdlib.h>

#include "enter.h"
#include "window.h"
#include "utils.h"
#include "png.h"

static void compute_shift(unsigned long mask,
		unsigned char *left_shift,
		unsigned char *right_shift) {
	*left_shift = 0;
	*right_shift = 8;
	if (mask != 0) {
		while ((mask & 0x01) == 0) {
			(*left_shift)++;
			mask >>= 1;
		}
		while ((mask & 0x01) == 1) {
			(*right_shift)--;
			mask >>= 1;
		}
	}
}


static int load_image(display_t *display, Pixmap pixmap, const char *filename)
{
	int i, j;
	unsigned char *rgb = NULL;
	unsigned char *alpha = NULL;
	int width, height;
	char *pixmap_data;
	int size;
	XImage *image;
	XVisualInfo *visual_info;
	XVisualInfo tmp_inf;
	int visual_enteries;
	unsigned long ipos = 0;

	if (!read_png(filename,&width,&height,&rgb,&alpha)) {
		return FALSE;
	}

	switch(display->depth) {
	case 32:
	case 24:
		size = 4 * width * height;
		break;
	case 16:
	case 15:
		size = 2 * width * height;
	break;
	default:
		size = width * height;
	}

	pixmap_data = (char*)xmalloc(size);

	image = XCreateImage(display->dpy,display->visual,display->depth,ZPixmap,0,
			pixmap_data, width, height, 8, 0);

	tmp_inf.visualid = XVisualIDFromVisual(display->visual),
	visual_info = XGetVisualInfo(display->dpy,VisualIDMask,
			&tmp_inf, &visual_enteries);

	switch (visual_info->class) {
	case TrueColor: {
		unsigned char red_left_shift;
		unsigned char red_right_shift;
		unsigned char green_left_shift;
		unsigned char green_right_shift;
		unsigned char blue_left_shift;
		unsigned char blue_right_shift;

		compute_shift(visual_info->red_mask, &red_left_shift,
			&red_right_shift);
		compute_shift(visual_info->green_mask, &green_left_shift,
			&green_right_shift);
		compute_shift(visual_info->blue_mask, &blue_left_shift,
			&blue_right_shift);

		unsigned long pixel;
		unsigned long red, green, blue;
		
		for (j = 0; j < height; j++) {
			for (i = 0; i < width; i++) {
				red = (unsigned long)
					rgb[ipos++] >> red_right_shift;
				green = (unsigned long)
					rgb[ipos++] >> green_right_shift;
				blue = (unsigned long)
					rgb[ipos++] >> blue_right_shift;

				pixel = (((red << red_left_shift) & visual_info->red_mask)
					| ((green << green_left_shift)
					& visual_info->green_mask)
					| ((blue << blue_left_shift)
					& visual_info->blue_mask));

				XPutPixel(image, i, j, pixel);
			}
		}
		}
		break;
	default:
		XFree(visual_info);
		XDestroyImage(image);

		return FALSE;
	}

	XPutImage(display->dpy, pixmap, display->gc, image, 0, 0, 0, 0, width, height);

	XFree(visual_info);
	XDestroyImage(image);

	return TRUE;
}

window_t* window_init(display_t *display, cfg_t *conf)
{
	unsigned long color;

	window_t *window  = xmalloc(sizeof *window);
	if (!window)
		return NULL;

	window->x = 0;
	window->y = 0;
	window->width = display->width;
	window->height = display->height;
	window->display = display;
	window->conf = conf;

	color = BlackPixel(display->dpy,display->screen);

	window->win = XCreateSimpleWindow(display->dpy, display->root,
		window->x, window->y, window->width, window->height,
		0, color, color);
	
	window->background = XCreatePixmap(display->dpy,window->win,
			window->width,window->height,display->depth);

	if (!load_image(display,window->background,"image.png")) {
		free(window);
		return NULL;
	}

	return window;
}

void window_show(window_t *window)
{
	display_t *display = window->display;
	
	XSelectInput(display->dpy, window->win, ExposureMask | KeyPressMask);

	XSetWindowBackgroundPixmap(display->dpy, window->win, window->background);

	XMapWindow(display->dpy, window->win);
	XMoveWindow(display->dpy, window->win, window->x, window->y);

	XGrabKeyboard(display->dpy, window->win, False, GrabModeAsync, GrabModeAsync, CurrentTime);

	XFlush(display->dpy);
}
