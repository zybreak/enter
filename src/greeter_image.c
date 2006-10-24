#include <X11/Xlib.h>
#include <X11/Xmu/WinUtil.h>

#include <stdio.h>
#include <stdlib.h>
#include <png.h>

#include "enter.h"
#include "greeter_image.h"
#include "utils.h"

static int read_png(const char *filename, int *width, int *height, unsigned char **rgb, 
	 unsigned char **alpha)
{
    FILE *infile = fopen(filename, "rb");
    if (!infile)
	    return FALSE;

    png_structp png_ptr;
    png_infop info_ptr;
    png_bytepp row_pointers;

    unsigned char *ptr = NULL;
    png_uint_32 w, h;
    int bit_depth, color_type, interlace_type;
    int i;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, 
                                     (png_voidp) NULL, 
                                     (png_error_ptr) NULL, 
                                     (png_error_ptr) NULL);
    if (!png_ptr) {
        fclose(infile);
        return FALSE;
    }
  
    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL, 
                                (png_infopp) NULL);
        fclose(infile);
        return FALSE;
    }
  
    if (setjmp(png_ptr->jmpbuf)) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        fclose(infile);
        return FALSE;
    }
  
    png_init_io(png_ptr, infile);
    png_read_info(png_ptr, info_ptr);

    png_get_IHDR(png_ptr, info_ptr, &w, &h, &bit_depth, &color_type,
                 &interlace_type, (int *) NULL, (int *) NULL);

    *width = (int) w;
    *height = (int) h;
    
    if (color_type == PNG_COLOR_TYPE_RGB_ALPHA
	|| color_type == PNG_COLOR_TYPE_GRAY_ALPHA) {
	*alpha = xmalloc(*width * *height);
	if (!*alpha) {
	    fprintf(stderr, "Can't allocate memory for alpha channel in PNG file.\n");
	    return FALSE; 
	}
    }

    /* Change a paletted/grayscale image to RGB */
    if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8) 
        png_set_expand(png_ptr);

    /* Change a grayscale image to RGB */
    if (color_type == PNG_COLOR_TYPE_GRAY 
        || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
        png_set_gray_to_rgb(png_ptr);

    /* If the PNG file has 16 bits per channel, strip them down to 8 */
    if (bit_depth == 16)
	    png_set_strip_16(png_ptr);

    /* use 1 byte per pixel */
    png_set_packing(png_ptr);

    row_pointers = xmalloc(*height * sizeof(png_bytep));
    if (!row_pointers) {
        fprintf(stderr, "Can't allocate memory for PNG file.\n");
        return FALSE;
    }

    for (i = 0; i < *height; i++) {
        row_pointers[i] = xmalloc(4 * *width);
        if (!row_pointers) {
            fprintf(stderr, "Can't allocate memory for PNG line.\n");
            return FALSE;
        }
    }

    png_read_image(png_ptr, row_pointers);

    *rgb = xmalloc(3 * *width * *height);
    if (!*rgb) {
        fprintf(stderr, "Can't allocate memory for PNG file.\n");
        return FALSE;
    }

    if (!*alpha) {
	ptr = *rgb;
	for (i = 0; i < *height; i++) {
	    memcpy(ptr, row_pointers[i], 3 * *width);
	    ptr += 3 * *width;
	}
    } else {
	int j;
	ptr = *rgb;
	for (i = 0; i < *height; i++) {
	    int ipos = 0;
	    for (j = 0; j < *width; j++) {
		*ptr++ = row_pointers[i][ipos++];
		*ptr++ = row_pointers[i][ipos++];
		*ptr++ = row_pointers[i][ipos++];
		*alpha[i * *width + j] = row_pointers[i][ipos++];
	    }
	}
    }

    png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);

    for (i = 0; i < *height; i++)
	    free(row_pointers[i]);
    free(row_pointers);

    fclose(infile);

    return TRUE;
}

static void compute_shift(unsigned long mask,
		unsigned char *left_shift,
		unsigned char *right_shift)
{
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

XImage* image_load(display_t *display, const char *filename)
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
		return NULL;
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
	case PseudoColor: {
		XColor xc;
		int ncolors = 256;
		XColor colors[ncolors];
		int closest_color[ncolors];
		Colormap colormap = DefaultColormap(display->dpy,display->screen);

		xc.flags = DoRed | DoGreen | DoBlue;
		for (i = 0; i < ncolors; i++)
			colors[i].pixel = (unsigned long) i;
		XQueryColors(display->dpy,colormap,colors,ncolors);

		for (i = 0; i < ncolors; i++) {
			xc.red = (i & 0xe0) << 8;
			xc.green = (i & 0x1c) << 11;
			xc.blue = (i & 0x03) << 14;
			
			double distance, distance_squared, min_distance = 0;
			for (j = 0; j < ncolors; j++) {
				distance = colors[j].red - xc.red;
				distance_squared = distance * distance;
				distance = colors[j].green - xc.green;
				distance_squared += distance * distance;
				distance = colors[j].blue - xc.blue;
				distance_squared += distance * distance;

				if ((j == 0) || (distance_squared <= min_distance)) {
					min_distance = distance_squared;
					closest_color[i] = j;
				}
			}
		}

		for (j = 0; j < height; j++) {
			for (i = 0; i < width; i++) {
				xc.red = (unsigned short) (rgb[ipos++] & 0xe0);
				xc.green = (unsigned short) (rgb[ipos++] & 0xe0);
				xc.blue = (unsigned short) (rgb[ipos++] & 0xc0);

				xc.pixel = xc.red | (xc.green >> 3) | (xc.blue >> 6);
				XPutPixel(image, i, j,
					colors[closest_color[xc.pixel]].pixel);
			}
		}
		}

		break;
	default:
		XFree(visual_info);
		XDestroyImage(image);

		return NULL;
	}

	XFree(visual_info);
	free(rgb);
	free(alpha);

	return image;
}

Pixmap image_to_pixmap(window_t *window, XImage *image, Pixmap pixmap)
{
	display_t *display = window->display;

	XPutImage(display->dpy, pixmap, display->gc, image, 0, 0, 0, 0, image->width, image->height);
	
	return pixmap;
}

