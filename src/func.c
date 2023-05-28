#include "header.h"

#include <stdlib.h>

void draw_rect(int x, int y, int width, int height, int color) {
    for (int i = x; i < x + width; i++)
        for (int j = y; j < y + height; j++)
            set_pixel(i, j, color);
}

void draw_line(int x1, int y1, int x2, int y2, int color) {
    int dx = x2 - x1;
    int dy = y2 - y1;

    if (dx == 0) {
        if (y1 > y2) {
            int tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        for (int i = y1; i < y2; i++) {
            set_pixel(x1, i, color);
        }
        return;
    }

    if (dy == 0) {
        if (x1 > x2) {
            int tmp = x1;
            x1 = x2;
            x2 = tmp;
        }
        for (int i = x1; i < x2; i++) {
            set_pixel(i, y1, color);
        }
        return;
    }

    double a = (double) dy / dx;
    double b = y1 - a * x1;

    if (abs(dx) > abs(dy)) {
        if (x1 > x2) {
            int tmp = x1;
            x1 = x2;
            x2 = tmp;
        }
        for (int i = x1; i < x2; i++) {
            set_pixel(i, (int) (a * i + b), color);
        }
    } else {
        if (y1 > y2) {
            int tmp = y1;
            y1 = y2;
            y2 = tmp;
        }
        for (int i = y1; i < y2; i++) {
            set_pixel((int) ((i - b) / a), i, color);
        }
    }
}
