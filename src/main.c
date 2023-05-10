#include "header.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define MAP_SIZE 13
#define PI 3.14159
#define ONK 1000.0

#define BLOCK_RESY 2
#define MINIMAP_SIZE 4

#define PLAYER_SPEED 5
#define ROT_SPEED 3
#define FOV (PI / 4)

#define RAW_SPEED 0.01

#define FLOOR_COLOR 0x000044
#define CEILING_COLOR 0x66FFFF

#define HALF_RESY (RESY / 2)

#define set_pixel(x, y, color) gui_screen_buffer[(y) * RESX + (x)] = (color)
#define float_part(x) (x - (int) x)

uint8_t MAP[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 6,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 6,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 6,
    2, 0, 0, 5, 5, 5, 5, 5, 5, 5, 0, 0, 6,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 6,
    2, 0, 0, 5, 5, 5, 5, 5, 5, 5, 0, 0, 6,
    2, 0, 0, 5, 0, 0, 0, 0, 0, 5, 0, 0, 6,
    2, 0, 0, 5, 0, 0, 0, 0, 0, 5, 0, 0, 6,
    2, 0, 0, 5, 5, 5, 5, 0, 0, 5, 0, 0, 6,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 6,
    2, 0, 0, 0, 0, 0, 0, 0, 0, 5, 0, 0, 6,
    2, 7, 7, 7, 7, 9, 7, 7, 7, 7, 7, 7, 6
};

#define TXR_SIZE 6
uint32_t WALL_TEXTURE[] = {
    0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00,
    0xFFFF00, 0xFFFFAA, 0xFFFFAA, 0xFFFFAA, 0xFFFFAA, 0xFFFF00,
    0xFFFF00, 0xFFFFAA, 0xFFFFAA, 0xFFFFAA, 0xFFFFAA, 0xFFFF00,
    0xFFFF00, 0xFFFFAA, 0xFFFFAA, 0xFFFFAA, 0xFFFFAA, 0xFFFF00,
    0xFFFF00, 0xFFFFAA, 0xFFFFAA, 0xFFFFAA, 0xFFFFAA, 0xFFFF00,
    0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00, 0xFFFF00,
};

uint32_t *gui_screen_buffer;

double get_distance(double x, double y, double dx, double dy, uint8_t *texture);
uint32_t texture_to_color(int texture);

void draw_rect(int x, int y, int width, int height, int color);

double closer_to_int(double x, double y) {
    double fx = float_part(x);
    double fy = float_part(y);

    if (fx < 0.5) fx = 1 - fx;
    if (fy < 0.5) fy = 1 - fy;

    if (fx < 0) fx = -fx;
    if (fy < 0) fy = -fy;

    return (fx < fy) ? x : y;
}

int main(int argc, char **argv) {
    double x = 5, y = 5;
    double rot = 0; // in radians

    double dx, dy, distance, rad_angle;
    int top, bottom, pressed;
    uint8_t key, texture;

    uint8_t key_state[4] = {0, 0, 0, 0};
    // z, q, s, d

    gui_screen_buffer = malloc(RESX * RESY * sizeof(uint32_t));

    int tick_count[4];
    tick_count[0] = get_ticks();
    tick_count[3] = 0;

    gui_init();

    while (1) {
        tick_count[1] = get_ticks() - tick_count[0];
        tick_count[0] = get_ticks();

        for (int i = 0; i < RESX; i++) {            
            rad_angle = rot + (FOV / 2) - (FOV * i / RESX);

            dx = cos(rad_angle);
            dy = sin(rad_angle);
            
            distance = get_distance(x, y, dx, dy, &texture);

            top = (int) (HALF_RESY - (HALF_RESY * BLOCK_RESY / distance));
            bottom = (int) (HALF_RESY + (HALF_RESY * BLOCK_RESY / distance));

            if (texture != 5) {
                for (int j = 0; j < RESY; j++) {
                    if (j < top) set_pixel(i, j, CEILING_COLOR);
                    else if (j > bottom) set_pixel(i, j, FLOOR_COLOR);
                    else set_pixel(i, j, texture_to_color(texture));
                }
                continue;
            }

            double map_x = x + dx * distance;
            double map_y = y + dy * distance;

            double good = closer_to_int(map_x, map_y);
            int x_part = (good - ((int) good)) * TXR_SIZE;

            uint32_t color = ((int) (float_part(map_x) * 250)) << 16 | ((int) (float_part(map_y) * 250)) << 8 | 0x00;

            for (int j = 0; j < RESY; j++) {

                if (j < top) set_pixel(i, j, CEILING_COLOR);
                else if (j > bottom) set_pixel(i, j, FLOOR_COLOR);
                else {
                    int y_part = (j - top) * TXR_SIZE / (bottom - top);
                    // printf("x_part: %d, y_part: %d\n", x_part, y_part);
                    set_pixel(i, j, WALL_TEXTURE[x_part + y_part * TXR_SIZE]);
                }
                    
            }
        }

        // draw minimap

        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                draw_rect(RESX - MINIMAP_SIZE * MAP_SIZE + i * MINIMAP_SIZE, j * MINIMAP_SIZE, MINIMAP_SIZE, MINIMAP_SIZE, texture_to_color(MAP[i + j * MAP_SIZE]));
                if (i == (int) x && j == (int) y)
                    draw_rect(RESX - MINIMAP_SIZE * MAP_SIZE + i * MINIMAP_SIZE, j * MINIMAP_SIZE, MINIMAP_SIZE, MINIMAP_SIZE, 0xFFFFFF);
                if (i == (int)(x + cos(rot) * 2) && j == (int)(y + sin(rot) * 2))
                    draw_rect(RESX - MINIMAP_SIZE * MAP_SIZE + i * MINIMAP_SIZE, j * MINIMAP_SIZE, MINIMAP_SIZE / 2, MINIMAP_SIZE / 2, 0x00FF00);
            }
        }

        // draw fps
        draw_rect(0, 0, tick_count[1] * 2, 7, 0x880000);
        draw_rect(0, 0, (tick_count[1] - tick_count[3]) * 2, 7, 0xCC0000);

        get_key(&pressed, &key);

        if (key == 122) {   // z
            key_state[0] = pressed;
        } else if (key == 113) {    // q
            key_state[1] = pressed;
        } else if (key == 115) {    // s
            key_state[2] = pressed;
        } else if (key == 100) {    // d
            key_state[3] = pressed;
        }

        if (key_state[0]) { // go forward
            x += cos(rot) * PLAYER_SPEED * tick_count[1] / ONK;
            y += sin(rot) * PLAYER_SPEED * tick_count[1] / ONK;
        }

        if (key_state[2]) { // go backward
            x -= cos(rot) * PLAYER_SPEED * tick_count[1] / ONK;
            y -= sin(rot) * PLAYER_SPEED * tick_count[1] / ONK;
        }

        if (key_state[1]) { // look left
            rot += ROT_SPEED * tick_count[1] / ONK;
        }

        if (key_state[3]) { // look right
            rot -= ROT_SPEED * tick_count[1] / ONK;
        }

        // if (x < 1) x = 1;
        // if (y < 1) y = 1;
        // if (x > MAP_SIZE - 2) x = MAP_SIZE - 2;
        // if (y > MAP_SIZE - 2) y = MAP_SIZE - 2;

        if (rot > PI) rot -= 2 * PI;
        if (rot < -PI) rot += 2 * PI;

        printf("fps: %03d, keys[%d, %d, %d, %d], rot: %f, x: %f, y: %f\n",
                1000 / tick_count[1],
                key_state[0],
                key_state[1],
                key_state[2],
                key_state[3],
                rot, x, y
        );

        tick_count[2] = get_ticks();
        gui_draw_frame();
        tick_count[3] = get_ticks() - tick_count[2];        
    }

    return 0;
}

void draw_rect(int x, int y, int width, int height, int color) {
    for (int i = x; i < x + width; i++)
        for (int j = y; j < y + height; j++)
            set_pixel(i, j, color);
}

uint32_t texture_to_color(int texture) {
    switch (texture) {
        case 0: return 0x000000;
        case 1: return 0x0000AA;
        case 2: return 0x00AA00;
        case 3: return 0x00AAAA;
        case 4: return 0xAA0000;
        case 5: return 0xAA00AA;
        case 6: return 0xAA5500;
        case 7: return 0xAAAAAA;
        case 8: return 0x555555;
        case 9: return 0x5555FF;
        case 10: return 0x55FF55;
        case 11: return 0x55FFFF;
        case 12: return 0xFF5555;
        case 13: return 0xFF55FF;
        case 14: return 0xFFFF55;
        case 15: return 0xFFFFFF;
    }
    return 0;
}

double get_distance(double x, double y, double dx, double dy, uint8_t *texture) {
    int map_x;
    int map_y;

    double distance = 0;
    do {
        distance += RAW_SPEED;
        map_x = (int) (x + dx * distance);
        map_y = (int) (y + dy * distance);
        if (map_x < 0 || map_x >= MAP_SIZE || map_y < 0 || map_y >= MAP_SIZE) {
            *texture = 0;
            return distance;
        }
    } while (!MAP[map_x + map_y * MAP_SIZE]);

    *texture = MAP[map_x + map_y * MAP_SIZE];
    return distance;
}
