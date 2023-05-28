#ifndef H314_H
#define H314_H

#define KEYQUEUE_SIZE 16

#define RESX 640
#define RESY 480

#include "header.h"

#include <stdint.h>

#define set_pixel(x, y, color) gui_screen_buffer[(y) * RESX + (x)] = (color)
#define float_part(x) (x - (int) x)

extern uint32_t *gui_screen_buffer;

void gui_init();
void gui_draw_frame();
void sleep_ms(uint32_t ms);
uint32_t get_ticks();
int get_key(int* pressed, uint8_t* Key);
void set_window_title(char *title);

uint32_t *open_bmp(char *path);

void draw_rect(int x, int y, int width, int height, int color);
void draw_line(int x1, int y1, int x2, int y2, int color);

#endif
