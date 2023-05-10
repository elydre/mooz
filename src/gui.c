#include "header.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>

static Display *display;
static Window window;
static int screen;
static GC gc;
static XImage *image;

extern uint32_t* gui_screen_buffer;

static uint16_t key_queue[KEYQUEUE_SIZE];
static uint32_t key_queue_write_index = 0;
static uint32_t key_queue_read_index = 0;

static void add_key_to_queue(int pressed, uint32_t key_code) {
    unsigned char key = key_code & 0xFF;

    uint16_t key_data = (pressed << 8) | key;

    key_queue[key_queue_write_index] = key_data;
    key_queue_write_index++;
    key_queue_write_index %= KEYQUEUE_SIZE;
}

void gui_init() {
    memset(key_queue, 0, KEYQUEUE_SIZE * sizeof(uint16_t));

    // window creation

    display = XOpenDisplay(NULL);

    screen = DefaultScreen(display);

    int blackColor = BlackPixel(display, screen);
    int whiteColor = WhitePixel(display, screen);

    XSetWindowAttributes attr;
    memset(&attr, 0, sizeof(XSetWindowAttributes));
    attr.event_mask = ExposureMask | KeyPressMask;
    attr.background_pixel = BlackPixel(display, screen);

    int depth = DefaultDepth(display, screen);

    window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, RESX, RESY, 0, blackColor, blackColor);

    XSelectInput(display, window, StructureNotifyMask | KeyPressMask | KeyReleaseMask);

    XMapWindow(display, window);

    gc = XCreateGC(display, window, 0, NULL);

    XSetForeground(display, gc, whiteColor);

    XkbSetDetectableAutoRepeat(display, 1, 0);

    // Wait for the MapNotify event

    while (1) {
        XEvent e;
        XNextEvent(display, &e);
        if (e.type == MapNotify) {
            break;
        }
    }

    image = XCreateImage(display, DefaultVisual(display, screen), depth, ZPixmap, 0, (char *) gui_screen_buffer, RESX, RESX, 32, 0);
}


void gui_draw_frame() {
    int sym;
    if (display) {
        while (XPending(display) > 0) {
            XEvent e;
            XNextEvent(display, &e);
            if (e.type == KeyPress) {
                sym = XkbKeycodeToKeysym(display, e.xkey.keycode, 0, 0);
                // printf("KeyPress:%d sym:%d\n", e.xkey.key_code, sym);
                add_key_to_queue(1, sym);
            }
            else if (e.type == KeyRelease) {
                sym = XkbKeycodeToKeysym(display, e.xkey.keycode, 0, 0);
                // printf("KeyRelease:%d sym:%d\n", e.xkey.key_code, sym);
                add_key_to_queue(0, sym);
            }
        }

        XPutImage(display, window, gc, image, 0, 0, 0, 0, RESX, RESY);

        // XFlush(display);
    }

    // puts("frame");
}

void sleep_ms(uint32_t ms) {
    usleep(ms * 1000);
}

uint32_t get_ticks() {
    struct timeval  tp;
    struct timezone tzp;

    gettimeofday(&tp, &tzp);

    return (tp.tv_sec * 1000) + (tp.tv_usec / 1000); /* return milliseconds */
}

int get_key(int* pressed, unsigned char* Key) {
    if (key_queue_read_index == key_queue_write_index) {
        //key queue is empty

        return 0;
    } else {
        uint16_t key_data = key_queue[key_queue_read_index];
        key_queue_read_index++;
        key_queue_read_index %= KEYQUEUE_SIZE;

        *pressed = key_data >> 8;
        *Key = key_data & 0xFF;

        return 1;
    }
}

void set_window_title(char *title) {
    XChangeProperty(display, window, XA_WM_NAME, XA_STRING, 8, PropModeReplace, (const unsigned char *) title, strlen(title));
}
