#include "header.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/XKBlib.h>

static Display *s_Display;
static Window s_Window;
static int s_Screen;
static GC s_Gc;
static XImage *s_Image;

extern uint32_t* gui_screen_buffer;

static unsigned short s_KeyQueue[KEYQUEUE_SIZE];
static unsigned int s_KeyQueueWriteIndex = 0;
static unsigned int s_KeyQueueReadIndex = 0;

static void add_key_to_queue(int pressed, unsigned int keyCode) {
    unsigned char key = keyCode & 0xFF;

    unsigned short key_data = (pressed << 8) | key;

    s_KeyQueue[s_KeyQueueWriteIndex] = key_data;
    s_KeyQueueWriteIndex++;
    s_KeyQueueWriteIndex %= KEYQUEUE_SIZE;
}

void gui_init() {
    memset(s_KeyQueue, 0, KEYQUEUE_SIZE * sizeof(unsigned short));

    // window creation

    s_Display = XOpenDisplay(NULL);

    s_Screen = DefaultScreen(s_Display);

    int blackColor = BlackPixel(s_Display, s_Screen);
    int whiteColor = WhitePixel(s_Display, s_Screen);

    XSetWindowAttributes attr;
    memset(&attr, 0, sizeof(XSetWindowAttributes));
    attr.event_mask = ExposureMask | KeyPressMask;
    attr.background_pixel = BlackPixel(s_Display, s_Screen);

    int depth = DefaultDepth(s_Display, s_Screen);

    s_Window = XCreateSimpleWindow(s_Display, DefaultRootWindow(s_Display), 0, 0, RESX, RESY, 0, blackColor, blackColor);

    XSelectInput(s_Display, s_Window, StructureNotifyMask | KeyPressMask | KeyReleaseMask);

    XMapWindow(s_Display, s_Window);

    s_Gc = XCreateGC(s_Display, s_Window, 0, NULL);

    XSetForeground(s_Display, s_Gc, whiteColor);

    XkbSetDetectableAutoRepeat(s_Display, 1, 0);

    // Wait for the MapNotify event

    while(1) {
        XEvent e;
        XNextEvent(s_Display, &e);
        if (e.type == MapNotify)
        {
            break;
        }
    }

    s_Image = XCreateImage(s_Display, DefaultVisual(s_Display, s_Screen), depth, ZPixmap, 0, (char *)gui_screen_buffer, RESX, RESX, 32, 0);
}


void gui_draw_frame() {
    int sym;
    if (s_Display) {
        while (XPending(s_Display) > 0) {
            XEvent e;
            XNextEvent(s_Display, &e);
            if (e.type == KeyPress) {
                sym = XkbKeycodeToKeysym(s_Display, e.xkey.keycode, 0, 0);
                printf("KeyPress:%d sym:%d\n", e.xkey.keycode, sym);
                add_key_to_queue(1, sym);
            }
            else if (e.type == KeyRelease) {
                sym = XkbKeycodeToKeysym(s_Display, e.xkey.keycode, 0, 0);
                printf("KeyRelease:%d sym:%d\n", e.xkey.keycode, sym);
                add_key_to_queue(0, sym);
            }
        }

        XPutImage(s_Display, s_Window, s_Gc, s_Image, 0, 0, 0, 0, RESX, RESY);

        // XFlush(s_Display);
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
    if (s_KeyQueueReadIndex == s_KeyQueueWriteIndex) {
        //key queue is empty

        return 0;
    } else {
        unsigned short key_data = s_KeyQueue[s_KeyQueueReadIndex];
        s_KeyQueueReadIndex++;
        s_KeyQueueReadIndex %= KEYQUEUE_SIZE;

        *pressed = key_data >> 8;
        *Key = key_data & 0xFF;

        return 1;
    }
}

void set_window_title(char *title) {
    XChangeProperty(s_Display, s_Window, XA_WM_NAME, XA_STRING, 8, PropModeReplace, (const unsigned char *) title, strlen(title));
}
