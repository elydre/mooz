#ifndef H314_H
#define H314_H

#define KEYQUEUE_SIZE 16

#define RESX 640
#define RESY 480

#include "header.h"

#include <stdint.h>

void gui_init();
void gui_draw_frame();
void sleep_ms(uint32_t ms);
uint32_t get_ticks();
int get_key(int* pressed, uint8_t* Key);
void set_window_title(char *title);

#endif