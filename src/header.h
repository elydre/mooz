#ifndef H314_H
#define H314_H

#define KEYQUEUE_SIZE 16

#define RESX 640
#define RESY 480

#include "header.h"

#include <stdint.h>

#define set_pixel(x, y, color) kalos_set_pixel(x, y, (color >> 16) & 0xFF, (color >> 8) & 0xFF, color & 0xFF)
#define float_part(x) (x - (int) x)

extern uint32_t *gui_screen_buffer;

uint32_t *open_bmp(char *path);

#endif
