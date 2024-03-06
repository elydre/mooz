#include "kalos.h"
#ifdef __linux__

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <time.h>

Display *kalos_display;
Window kalos_window;
Pixmap kalos_buffer;
int kalos_screen;

GC kalos_gc_buffer;

XIM kalos_xim;
XIC kalos_xic;

int kalos_width = 400;
int kalos_height = 400;
#define kalos_window_mask (KeyPressMask | KeyReleaseMask)

int kalos_init() {
    kalos_display = XOpenDisplay(NULL);
    if (NULL == kalos_display)
        return KALOS_INIT_FAIL;
    kalos_screen = DefaultScreen(kalos_display);
    kalos_window = XCreateSimpleWindow(kalos_display, RootWindow(kalos_display, kalos_screen), 0, 0,
        kalos_width, kalos_height, 0, 0, WhitePixel(kalos_display, kalos_screen));
    XSelectInput(kalos_display, kalos_window, kalos_window_mask);
    kalos_buffer = XCreatePixmap(kalos_display, kalos_window, kalos_width, kalos_height, DefaultDepth(kalos_display, 0));
    kalos_xim = XOpenIM(kalos_display, 0, 0, 0);
    kalos_xic = XCreateIC(kalos_xim, XNInputStyle, XIMPreeditNothing | XIMStatusNothing, NULL);
    kalos_gc_buffer = XCreateGC(kalos_display, kalos_buffer, 0, NULL);
    return KALOS_INIT_SUCCESS;
}

void kalos_show_window() {
    XSelectInput(kalos_display, kalos_window, StructureNotifyMask);
    XMapWindow(kalos_display, kalos_window);
    XEvent e;
    do {
        XNextEvent(kalos_display, &e);
    } while (e.type != MapNotify);
    XSelectInput(kalos_display, kalos_window, kalos_window_mask);
    XFlush(kalos_display);

}

void kalos_update_window() {
    GC gc = XCreateGC(kalos_display, kalos_window, 0, NULL);
    XCopyArea(kalos_display, kalos_buffer, kalos_window, gc, 0, 0, kalos_width, kalos_height, 0, 0);
    XFreeGC(kalos_display, gc);
    XFlush(kalos_display);
}

void kalos_set_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
        XColor color;
    color.red = r * 257;    // Scale the color values
    color.green = g * 257;
    color.blue = b * 257;
    color.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(kalos_display, DefaultColormap(kalos_display, kalos_screen), &color)) {
        return;
    }
    XSetForeground(kalos_display, kalos_gc_buffer, color.pixel);
    XDrawPoint(kalos_display, kalos_buffer, kalos_gc_buffer, x, y);
}

void kalos_fill_window(unsigned char r, unsigned char g, unsigned char b) {
    XColor color;
    color.red = r * 257;
    color.green = g * 257;
    color.blue = b * 257;
    color.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(kalos_display, DefaultColormap(kalos_display, kalos_screen), &color)) {
        XFreeGC(kalos_display, kalos_gc_buffer);
        return ;
    }
    XSetForeground(kalos_display, kalos_gc_buffer, color.pixel);
    XFillRectangle(kalos_display, kalos_buffer, kalos_gc_buffer, 0, 0, kalos_width, kalos_height);
}

void kalos_fill_rect(int x, int y, int h, int w, unsigned char r, unsigned char g, unsigned char b) {
    XColor color;
    color.red = r * 257;
    color.green = g * 257;
    color.blue = b * 257;
    color.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(kalos_display, DefaultColormap(kalos_display, kalos_screen), &color)) {
        XFreeGC(kalos_display, kalos_gc_buffer);
        return ;
    }
    XSetForeground(kalos_display, kalos_gc_buffer, color.pixel);
    XFillRectangle(kalos_display, kalos_buffer, kalos_gc_buffer, x, y, w, h);
}

void kalos_draw_line(int x1, int y1, int x2, int y2, unsigned char r, unsigned char g, unsigned char b) {
    XColor color;
    color.red = r * 257;
    color.green = g * 257;
    color.blue = b * 257;
    color.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(kalos_display, DefaultColormap(kalos_display, kalos_screen), &color)) {
        XFreeGC(kalos_display, kalos_gc_buffer);
        return ;
    }
    XSetForeground(kalos_display, kalos_gc_buffer, color.pixel);
    XDrawLine(kalos_display, kalos_buffer, kalos_gc_buffer, x1, y1, x2, y2);
}

void kalos_draw_disk(int x, int y, int radius, unsigned char r, unsigned char g, unsigned char b) {
    XColor color;
    color.red = r * 257;
    color.green = g * 257;
    color.blue = b * 257;
    color.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(kalos_display, DefaultColormap(kalos_display, kalos_screen), &color)) {
        XFreeGC(kalos_display, kalos_gc_buffer);
        return ;
    }
    XSetForeground(kalos_display, kalos_gc_buffer, color.pixel);
    XFillArc(kalos_display, kalos_buffer, kalos_gc_buffer, x - radius, y - radius, 2 * radius, 2 * radius, 0, 360 * 64);
}

void kalos_draw_circle(int x, int y, int radius, unsigned char r, unsigned char g, unsigned char b) {
    XColor color;
    color.red = r * 257;
    color.green = g * 257;
    color.blue = b * 257;
    color.flags = DoRed | DoGreen | DoBlue;

    if (!XAllocColor(kalos_display, DefaultColormap(kalos_display, kalos_screen), &color)) {
        XFreeGC(kalos_display, kalos_gc_buffer);
        return ;
    }
    XSetForeground(kalos_display, kalos_gc_buffer, color.pixel);
    XDrawArc(kalos_display, kalos_buffer, kalos_gc_buffer, x - radius, y - radius, 2 * radius, 2 * radius, 0, 360 * 64);
}

void __kalos_handle_window_resize(int new_width, int new_height) {
    int old_width = kalos_width;
    int old_height = kalos_height;
    kalos_width = new_width;
    kalos_height = new_height;
    Pixmap new_buffer = XCreatePixmap(kalos_display, kalos_window, kalos_width, kalos_height, DefaultDepth(kalos_display, 0));
    XCopyArea(kalos_display, kalos_buffer, new_buffer, kalos_gc_buffer, 0, 0, old_width, old_height, 0, 0);
    XFreePixmap(kalos_display, kalos_buffer);
    kalos_buffer = new_buffer;
}

int kalos_get_height() {
    return kalos_height;
}
int kalos_get_width() {
    return kalos_width;
}

void kalos_update_events() {
    XEvent event;

    XWindowAttributes windowAttributes;
    XGetWindowAttributes(kalos_display, kalos_window, &windowAttributes);

    if (windowAttributes.width != kalos_width || windowAttributes.height != kalos_height)
        __kalos_handle_window_resize(windowAttributes.width, windowAttributes.height);

    KeySym keysym;
    char buffer[5] = {0, 0, 0, 0, 0};
    while (XPending(kalos_display)) {
        XNextEvent(kalos_display, &event);
        /* Gestion des événements */
        switch (event.type) {
            case KeyRelease:
            case KeyPress:
                KeySym keySym2 = XkbKeycodeToKeysym(kalos_display, event.xkey.keycode, 0, event.xkey.state & ShiftMask ? 1 : 0);
                int key = -1;
                if (keySym2 == XK_Shift_L || keySym2 == XK_Shift_R)
                    key = KEY_SHIFT;
                else if (keySym2 == XK_Up)
                    key = KEY_ARROW_UP;
                else if (keySym2 == XK_Down)
                    key = KEY_ARROW_DOWN;
                else if (keySym2 == XK_Left)
                    key = KEY_ARROW_LEFT;
                else if (keySym2 == XK_Right)
                    key = KEY_ARROW_RIGHT;
                else if (keySym2 == XK_Escape)
                    key = KEY_ESC;
                else if (keySym2 == XK_Control_L)
                    key = KEY_CTRL_L;
                else if (keySym2 == XK_Control_R)
                    key = KEY_CTRL_R;
                else if (keySym2 == XK_Return)
                    key = KEY_ENTER;
                else if (keySym2 == XK_BackSpace)
                    key = KEY_ERASE;
                else if (keySym2 == XK_Alt_L || keySym2 == XK_Alt_R)
                    key = KEY_ALT;
                if (key != -1) {
                    kalos_events_len++;
                    kalos_events[kalos_events_len - 1].key[0] = key;
                    kalos_events[kalos_events_len - 1].key[1] = 0;
                    kalos_events[kalos_events_len - 1].key[2] = 0;
                    kalos_events[kalos_events_len - 1].key[3] = 0;
                    kalos_events[kalos_events_len - 1].key[4] = 0;
                    kalos_events[kalos_events_len - 1].is_pressed = event.type == KeyPress ? 3 : 4;
                }
                int old_state = event.type;
                event.type = KeyPress;
                if (key == -1 && Xutf8LookupString(kalos_xic, &event.xkey, buffer, 5, &keysym, NULL) > 0) {
                    event.type = old_state;
                    kalos_events_len++;
                    for(int i = 0; i < 5; i++)
                        kalos_events[kalos_events_len - 1].key[i] = buffer[i];
                    kalos_events[kalos_events_len - 1].is_pressed = event.type == KeyRelease ? 0 : 1;
                }
                break;
            default:
                break;
        }
    }
}

void kalos_end() {
    XFreePixmap(kalos_display, kalos_buffer);
    if (kalos_xic != NULL)
        XDestroyIC(kalos_xic);
    if (kalos_xim != NULL)
        XCloseIM(kalos_xim);
    XFreeGC(kalos_display, kalos_gc_buffer);
    XDestroyWindow(kalos_display, kalos_window);
    XCloseDisplay(kalos_display);
}

long long int kalos_get_time_ms() {
    struct timespec te;
    clock_gettime(CLOCK_REALTIME, &te);
    long long milliseconds = te.tv_sec * 1000LL + te.tv_nsec / 1000000LL;
    return milliseconds;
}

//void kalos_sleep_ms(long long int x) {
//    long long int end = x + kalos_get_time_ms();
//    while (kalos_get_time_ms() < end) {;}
//}


#endif