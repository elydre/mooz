#include "header.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

// player speed
#define PLAYER_SPEED 4
#define ROT_SPEED 2

// textures size
#define MAP_SIZE 13
#define TXR_SIZE 16

// render settings
#define FOV (PI / 4)
#define BLOCK_RESY (PI / 2)
#define RAW_SPEED 0.01

#define MINIMAP_SIZE 6

#define FLOOR_COLOR 0x11aa44
#define CEILING_COLOR 0x88bbff

// internal
#define HALF_RESY (RESY / 2)

#define PI 3.14159265358979323846
#define ONK 1000.0

#define get_distance(x, y, dx, dy) get_wall(x, y, dx, dy, NULL)

int8_t MAP[] = {
    1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1,
    1, 0, 0, 3, 3, 3, 3, 3, 3, 3, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
    1, 0, 0, 3, 3, 3, 3, 3, 3, 3, 0, 0, 1,
    1, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 1,
    1, 0, 0, 3, 0, 0, 0, 0, 0, 3, 0, 0, 1,
    1, 0, 0, 3, 0, 0, 1, 0, 0, 3, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 1,
    1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1
};

uint32_t *gui_screen_buffer;

int y_offset = 0;

uint8_t key_state[8];
// z, q, s, d, a, e, space, shift

double get_wall(double x, double y, double dx, double dy, uint8_t *texture);

uint32_t texture_to_color(int texture);

void move(double *player_x, double *player_y, double *rot, int fps);


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
    double x = 2, y = 2;
    double rot = 0; // in radians

    double dx, dy, distance, rad_angle;
    int top, bottom, pressed;
    uint8_t key, id;

    for (int i = 0; i < 6; i++)
        key_state[i] = 0;

    uint32_t *textures[3];
    textures[0] = open_bmp("img/br.bmp");
    textures[1] = open_bmp("img/stone.bmp");
    textures[2] = open_bmp("img/bricks.bmp");


    gui_screen_buffer = malloc(RESX * RESY * sizeof(uint32_t));

    int tick_count[4];
    tick_count[0] = get_ticks();
    tick_count[3] = 0;

    gui_init();

    while (1) {
        tick_count[1] = get_ticks() - tick_count[0];
        if (tick_count[1] == 0) {
            printf("sleeping\n");
            sleep_ms(1);
            continue;
        }

        tick_count[0] = get_ticks();

        // draw walls
        for (int i = 0; i < RESX; i++) {            
            rad_angle = rot + (FOV / 2) - (FOV * i / RESX);

            dx = cos(rad_angle);
            dy = sin(rad_angle);

            distance = get_wall(x, y, dx, dy, &id);

            top = (int) (HALF_RESY - (HALF_RESY * BLOCK_RESY / distance)) + y_offset;
            bottom = (int) (HALF_RESY + (HALF_RESY * BLOCK_RESY / distance)) + y_offset;

            if (bottom == top) continue;

            double map_x = x + dx * distance;
            double map_y = y + dy * distance;

            double good = closer_to_int(map_x, map_y);
            int x_part = (good - ((int) good)) * TXR_SIZE;

            if (x_part >= TXR_SIZE) x_part = TXR_SIZE - 1;
            if (x_part < 0) x_part = 0;

            for (int j = 0; j < RESY; j++) {
                if (j < top) set_pixel(i, j, CEILING_COLOR);
                else if (j > bottom) set_pixel(i, j, FLOOR_COLOR);
                else {
                    int y_part = (j - top) * TXR_SIZE / (bottom - top);
                    if (y_part >= TXR_SIZE) y_part = TXR_SIZE - 1;
                    if (y_part < 0) y_part = 0;
                    set_pixel(i, j, textures[id][x_part + y_part * TXR_SIZE]);
                }
            }
        }

        // draw minimap
        for (int i = 0; i < MAP_SIZE; i++) {
            for (int j = 0; j < MAP_SIZE; j++) {
                draw_rect(RESX - MINIMAP_SIZE * MAP_SIZE + i * MINIMAP_SIZE, j * MINIMAP_SIZE, MINIMAP_SIZE, MINIMAP_SIZE, texture_to_color(MAP[i + j * MAP_SIZE]));
            }
        }

        // draw player
        draw_rect(RESX - MINIMAP_SIZE * MAP_SIZE + x * MINIMAP_SIZE - MINIMAP_SIZE / 2, y * MINIMAP_SIZE - MINIMAP_SIZE / 2, MINIMAP_SIZE, MINIMAP_SIZE, 0xFFFF00);
        // draw direction
        int dir_x = RESX - MINIMAP_SIZE * MAP_SIZE + x * MINIMAP_SIZE + cos(rot) * 5 * MINIMAP_SIZE;
        int dir_y = y * MINIMAP_SIZE + sin(rot) * 5 * MINIMAP_SIZE;
        if (dir_x < RESX - MINIMAP_SIZE * MAP_SIZE) dir_x = RESX - MINIMAP_SIZE * MAP_SIZE;
        if (dir_x >= RESX) dir_x = RESX - 1;
        if (dir_y < 0) dir_y = 0;
        if (dir_y >= MINIMAP_SIZE * MAP_SIZE) dir_y = MINIMAP_SIZE * MAP_SIZE - 1;

        draw_line(RESX - MINIMAP_SIZE * MAP_SIZE + x * MINIMAP_SIZE, y * MINIMAP_SIZE, dir_x, dir_y, 0xFFFF00);

        get_key(&pressed, &key);


        if (key == 122) {           // z
            key_state[0] = pressed;
        } else if (key == 113) {    // q
            key_state[1] = pressed;
        } else if (key == 115) {    // s
            key_state[2] = pressed;
        } else if (key == 100) {    // d
            key_state[3] = pressed;
        } else if (key == 97) {     // a
            key_state[4] = pressed;
        } else if (key == 101) {    // e
            key_state[5] = pressed;
        } else if (key == 32) {     // space
            key_state[6] = pressed;
        } else if (key == 225) {    // shift
            key_state[7] = pressed;
        }

        move(&x, &y, &rot, tick_count[1]);

        if (rot > PI) rot -= 2 * PI;
        if (rot < -PI) rot += 2 * PI;

        printf("fps: %04d, keys[%d,%d,%d,%d,%d,%d,%d,%d], rot: %f, x: %f, y: %f\n",
                1000 / tick_count[1],
                key_state[0],
                key_state[1],
                key_state[2],
                key_state[3],
                key_state[4],
                key_state[5],
                key_state[6],
                key_state[7],
                rot, x, y
        );

        tick_count[2] = get_ticks();
        gui_draw_frame();
        tick_count[3] = get_ticks() - tick_count[2];
    }

    return 0;
}

uint32_t texture_to_color(int texture) {
    switch (texture) {
        case 0: return 0x000000;
        case 1: return 0x0000AA;
        case 2: return 0x00AA00;
        case 3: return 0x00AAAA;
        default: return 0xFF0000;
    }
    return 0;
}

double get_wall(double x, double y, double dx, double dy, uint8_t *id) {
    int map_x;
    int map_y;

    double distance = 0;

    do {
        distance += RAW_SPEED;
        map_x = (int) (x + dx * distance);
        map_y = (int) (y + dy * distance);

        if (map_x < 0 || map_x >= MAP_SIZE || map_y < 0 || map_y >= MAP_SIZE) {
            if (id != NULL) *id = 0;
            return distance - RAW_SPEED;
        }

    } while (!MAP[map_x + map_y * MAP_SIZE]);

    if (id != NULL) *id = MAP[map_x + map_y * MAP_SIZE] - 1;

    return distance - RAW_SPEED;
}

void move(double *player_x, double *player_y, double *rot, int fps) {
    double x = 0;
    double y = 0;

    double dx, dy, distance;

    dx = cos(*rot);
    dy = sin(*rot);

    if (key_state[0]) { // go forward
        // check collisions
        distance = get_distance(*player_x, *player_y, dx, dy);

        if (distance > PLAYER_SPEED * fps / ONK) {
            x += dx * PLAYER_SPEED * fps / ONK;
            y += dy * PLAYER_SPEED * fps / ONK;
        }
    }

    if (key_state[2]) { // go backward
        // check collisions
        distance = get_distance(*player_x, *player_y, -dx, -dy);

        if (distance > PLAYER_SPEED * fps / ONK) {
            x -= dx * PLAYER_SPEED * fps / ONK;
            y -= dy * PLAYER_SPEED * fps / ONK;
        }
    }

    if (key_state[4]) { // strafe left
        distance = get_distance(*player_x, *player_y, -dy, dx);

        if (distance > PLAYER_SPEED * fps / ONK) {
            x -= dy * PLAYER_SPEED * fps / ONK;
            y += dx * PLAYER_SPEED * fps / ONK;
        }
    }

    if (key_state[5]) { // strafe right
        distance = get_distance(*player_x, *player_y, dy, -dx);

        if (distance > PLAYER_SPEED * fps / ONK) {
            x += dy * PLAYER_SPEED * fps / ONK;
            y -= dx * PLAYER_SPEED * fps / ONK;
        }
    }

    if (key_state[1]) { // look left
        *rot += ROT_SPEED * fps / ONK;
    }

    if (key_state[3]) { // look right
        *rot -= ROT_SPEED * fps / ONK;
    }

    if (key_state[6]) { // jump
        y_offset += 1;
    }

    if (key_state[7]) { // squat
        y_offset -= 1;
    }

    if (!key_state[6] && !key_state[7]) {
        y_offset = 0;
    }

    // apply movement
    *player_x += x;
    *player_y += y;
}
