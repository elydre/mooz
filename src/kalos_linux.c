#include "kalos.h"
#define kalos_min(a, b) (a < b ? a : b)

#ifdef __linux__

#include <stdio.h>
#include <stdlib.h>
#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <time.h>

Display *kalos_display;
Window kalos_window;
XImage *kalos_buffer;
char *kalos_buffer_ptr;
int kalos_screen;

XIM kalos_xim;
XIC kalos_xic;

GC _window_gc;


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
    kalos_buffer_ptr = malloc(kalos_width*kalos_height*4);
    kalos_buffer = XCreateImage(kalos_display, DefaultVisual(kalos_display, DefaultScreen(kalos_display)), DefaultDepth(kalos_display, DefaultScreen(kalos_display)), ZPixmap, 0, kalos_buffer_ptr, kalos_width, kalos_height, 32, 0);
    // display windows
    XSelectInput(kalos_display, kalos_window, StructureNotifyMask);
    XMapWindow(kalos_display, kalos_window);
    XEvent e;
    do {
        XNextEvent(kalos_display, &e);
    } while (e.type != MapNotify);
    XSelectInput(kalos_display, kalos_window, kalos_window_mask);
    XFlush(kalos_display);
    _window_gc = XCreateGC(kalos_display, kalos_window, 0, NULL);
    return KALOS_INIT_SUCCESS;
}

void kalos_update_window() {
    XPutImage(kalos_display, kalos_window, _window_gc, kalos_buffer, 0, 0, 0, 0, kalos_width, kalos_height);
}

void kalos_set_pixel(int x, int y, unsigned char r, unsigned char g, unsigned char b) {
    if (x >= kalos_width || y >= kalos_height) return ;
    u_int32_t *ptr = (u_int32_t *)kalos_buffer_ptr;
    ptr[x+ y*kalos_width] = r<<16 | g << 8 | b;
}


void kalos_fill_window(unsigned char r, unsigned char g, unsigned char b) {
    long int col = r<<16 | g << 8 | b;
    u_int32_t *ptr = (u_int32_t *)kalos_buffer_ptr;
    for (int i = 0; i < kalos_width; i++) {
        for (int k = 0; k < kalos_height; k++) {
            ptr[i+ k*kalos_width] = col;
        }
    }
}

void kalos_fill_rect(int x, int y, int h, int w, unsigned char r, unsigned char g, unsigned char b) {
    long int col = r<<16 | g << 8 | b;
    u_int32_t *ptr = (u_int32_t *)kalos_buffer_ptr;
    int w_end = kalos_min(w, kalos_width - 1);
    int h_end = kalos_min(h, kalos_height - 1);
    for (int i = x; i < w_end; i++) {
        for (int k = y; k < h_end; k++) {
            ptr[i + k*kalos_width] = col;
        }
    }
}

void kalos_draw_line(int x0, int y0, int x1, int y1, unsigned char r, unsigned char g, unsigned char b) {
    
    x0 = kalos_min(x0, kalos_width - 1);
    x1 = kalos_min(x1, kalos_width - 1);
    y0 = kalos_min(y0, kalos_width - 1);
    y1 = kalos_min(y1, kalos_width - 1);

    long int color = r<<16 | g << 8 | b;
    int dx = x1 - x0;
    int dy = y1 - y0;
    int D = 2*dy - dx;
    int y = y0;

    if (x0 < x1) {
        for (int x = x0; x < x1; x++) {
            XPutPixel(kalos_buffer, x, y, color);
            if (D > 0) {
                y = y + 1;
                D = D - 2*dx;
            }
            D = D + 2*dy;
        }
    }
    else {
        for (int x = x1 - 1; x >= x0; x--) {
            XPutPixel(kalos_buffer, x, y, color);
            if (D > 0) {
                y = y + 1;
                D = D - 2*dx;
            }
            D = D + 2*dy;
        }
    }
        
}

void kalos_draw_disk(int x, int y, int radius, unsigned char r, unsigned char g, unsigned char b) {
    //XColor color;
    //color.red = r * 257;
    //color.green = g * 257;
    //color.blue = b * 257;
    //color.flags = DoRed | DoGreen | DoBlue;
//
    //if (!XAllocColor(kalos_display, DefaultColormap(kalos_display, kalos_screen), &color)) {
    //    XFreeGC(kalos_display, kalos_gc_buffer);
    //    return ;
    //}
    //XSetForeground(kalos_display, kalos_gc_buffer, color.pixel);
    //XFillArc(kalos_display, kalos_buffer, kalos_gc_buffer, x - radius, y - radius, 2 * radius, 2 * radius, 0, 360 * 64);
}

void kalos_draw_circle(int x, int y, int radius, unsigned char r, unsigned char g, unsigned char b) {
    //XColor color;
    //color.red = r * 257;
    //color.green = g * 257;
    //color.blue = b * 257;
    //color.flags = DoRed | DoGreen | DoBlue;
//
    //if (!XAllocColor(kalos_display, DefaultColormap(kalos_display, kalos_screen), &color)) {
    //    XFreeGC(kalos_display, kalos_gc_buffer);
    //    return ;
    //}
    //XSetForeground(kalos_display, kalos_gc_buffer, color.pixel);
    //XDrawArc(kalos_display, kalos_buffer, kalos_gc_buffer, x - radius, y - radius, 2 * radius, 2 * radius, 0, 360 * 64);
}

void __kalos_handle_window_resize(int new_width, int new_height) {

    int old_width = kalos_width;
    int old_height = kalos_height;
    kalos_width = new_width;
    kalos_height = new_height;
    char *new_kalos_buffer_ptr = malloc(kalos_width * kalos_height * 4);
    XImage *new_buffer = XCreateImage(kalos_display, DefaultVisual(kalos_display, DefaultScreen(kalos_display)), DefaultDepth(kalos_display, DefaultScreen(kalos_display)), ZPixmap, 0, new_kalos_buffer_ptr, kalos_width, kalos_height, 32, 0);
    for (int x = 0; x < old_width; x++) {
        for (int y = 0; y < old_height; y++) {
            new_kalos_buffer_ptr[x + y *kalos_width] = kalos_buffer_ptr[x + y*old_width];
        }
    }
    XDestroyImage(kalos_buffer);
    kalos_buffer = new_buffer;
    kalos_buffer_ptr = new_kalos_buffer_ptr;
}

int kalos_get_height() {
    return kalos_height;
}
int kalos_get_width() {
    return kalos_width;
}

void kalos_set_width() {
}

void kalos_update_events() {
    kalos_events_len = 0;
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
    XDestroyImage(kalos_buffer);

    if (kalos_xic != NULL)
        XDestroyIC(kalos_xic);
    if (kalos_xim != NULL)
        XCloseIM(kalos_xim);
    XFreeGC(kalos_display, _window_gc);
    XDestroyWindow(kalos_display, kalos_window);
    XCloseDisplay(kalos_display);
}

long long int kalos_get_time_ms() {
    struct timespec te;
    clock_gettime(CLOCK_REALTIME, &te);
    long long milliseconds = te.tv_sec * 1000LL + te.tv_nsec / 1000000LL;
    return milliseconds;
}


#endif
