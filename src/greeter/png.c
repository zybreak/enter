#include <stdio.h>
#include <stdlib.h>
#include <png.h>

#include "enter.h"
#include "utils.h"

int read_png(const char *filename, int *width, int *height, unsigned char **rgb, 
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
