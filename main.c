#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
/*xdraw_ascii.c*/
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <omp.h>
#include <malloc.h>
#include <math.h>
#include <stdlib.h>

#define BORDER 2
#define WIDTH 1024
#define HIGHT 1024

int main(int argc, char **argv) {
    Display *dpy;
    XInitThreads();
    printf("%d", omp_get_max_threads());
    omp_set_num_threads(4);
    Window w;
    Window root;
    int screen;
    unsigned long black, white;
    GC gc;
    char here[9];
    XEvent e;
    KeySym key;
    char string[9];
    int l = 0;
    dpy = XOpenDisplay("");
    root = DefaultRootWindow(dpy);
    screen = DefaultScreen(dpy);
    white = WhitePixel(dpy, screen);
    black = BlackPixel(dpy, screen);

    XPoint *points = malloc(sizeof(XPoint) * WIDTH * HIGHT) + 100;

    w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
    gc = XCreateGC(dpy, w, 0, NULL);

    XSelectInput(dpy, w, ButtonPressMask | ButtonReleaseMask | KeyPressMask);

    XMapWindow(dpy, w);
    Colormap cmap = DefaultColormap(dpy, 0);
    XColor color1;

    color1.red = 256 * 1;
    color1.green = 256 * 1;
    color1.blue = 256 * 1;
    printf("%lu", color1.pixel);
    XAllocColor(dpy, cmap, &color1);
    XSetForeground(dpy, gc, color1.pixel);
    int i, j;
#pragma omp parallel for private(color1, j)
    for (i = 0; i < WIDTH; i++) {
        //i++;
        for (j = 0; j < HIGHT; ++j) {
            //j++;
            color1.red = (int) (sqrt(abs((i - WIDTH / 2)) * abs((j - HIGHT / 2))) * 256) % (256 * 256);
            color1.green = (int) (sqrt((i - WIDTH / 2) * (i - WIDTH / 2)) * 256) % (256 * 256);
            color1.blue = (int) (sqrt((j - HIGHT / 2) * (j - HIGHT / 2)) * 256) % (256 * 256);
            color1.red = (cos((double) i / 60) + 1) * 256 * 128;
            color1.green = (cos((double) j / 60) + 1) * 256 * 128;
            color1.blue = (cos((double) (i + j) / 120) + 1) * 256 * 128;
            XAllocColor(dpy, cmap, &color1);
            XSetForeground(dpy, gc, color1.pixel);
            XDrawPoint(dpy, w, gc, i, j);
        }
    }
    while (1);

    while (1) {
        XNextEvent(dpy, &e);
        switch (e.type) {
            case ButtonPress:
                printf("x=%d, y=%d, button=%d\n", e.xbutton.x, e.xbutton.y, e.xbutton.button);
                if (e.xbutton.button == 3) {
                    return (0);
                }
                break;
            case ButtonRelease:
                //color1.red = 128;
                color1.blue = (e.xbutton.x * 256) % (256 * 256);
                color1.green = (e.xbutton.y * 256) % (256 * 256);
                XAllocColor(dpy, cmap, &color1);
                XSetForeground(dpy, gc, color1.pixel);
                XDrawPoint(dpy, w, gc, e.xbutton.x, e.xbutton.y);

                break;
            case KeyPress:
                if (XLookupString((XKeyEvent *) &e, string, 9, &key, NULL) == 1) {
                    here[0] = string[0];
                    here[1] = '\0';
                    XDrawString(dpy, w, gc, 30 + l, 200, here, strlen(here));
                    l += 10;
                }
        }
    }
}

#pragma clang diagnostic pop