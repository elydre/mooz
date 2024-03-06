#ifndef KALOS_H
#define KALOS_H

#if !defined(__WIN32) && !defined(__linux__)
	#define KALOS_UNSUPPORTED
#endif

#define KALOS_INIT_FAIL 1
#define KALOS_INIT_SUCCESS 0

#define KEY_ESC 1
#define KEY_CTRL_L 2
#define KEY_CTRL_R 3
#define KEY_ALT 4
#define KEY_ARROW_UP 5
#define KEY_ARROW_DOWN 6
#define KEY_ARROW_LEFT 7
#define KEY_ARROW_RIGHT 8
#define KEY_ENTER 9
#define KEY_ERASE 10
#define KEY_SHIFT 11

typedef struct kalos_event_s{
	char key[5];
	char is_pressed; // 0 :key_released, 1 :key_pressed, 2 :end_of_events
	// 3 :special_key_pressed (key[0] == key)(esc, shift...)
	// 4 :special_key_released (key[0] == key)(esc, shift...)
}kalos_event_t;

#define kalos_events_max_len 150

extern kalos_event_t kalos_events[kalos_events_max_len];
extern int kalos_events_len;

int kalos_init();

void kalos_show_window();

void kalos_update_window();

void kalos_set_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b);

void kalos_fill_window(unsigned char r, unsigned char g, unsigned char b);

void kalos_fill_rect(int x, int y, int h, int w, unsigned char r, unsigned char g, unsigned char b);

void kalos_draw_line(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b);

void kalos_draw_circle(int x, int y, int radius, unsigned char r, unsigned char g, unsigned char b);

void kalos_draw_disk(int x, int y, int radius, unsigned char r, unsigned char g, unsigned char b);

int kalos_get_height();

int kalos_get_width();

void kalos_update_events();

void kalos_end();

long long int kalos_get_time_ms();

void kalos_sleep_ms(long long int time);

#endif
