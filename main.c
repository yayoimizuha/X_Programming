#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
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
#include <tiff.h>
#include <png.h>
#include <jpeglib.h>
#include <jpegint.h>
#include <xorg/joystick-properties.h>
#include <time.h>
#include <unistd.h>

#define BORDER 2
#define WIDTH 512
#define HIGHT 512

int main(int argc, char **argv) {
    Display *dpy;
    XInitThreads();
    printf("%d", omp_get_max_threads());
    omp_set_num_threads(1);
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
    XColor color1, color2;

    color1.red = 256 * 123;
    color1.green = 256 * 31;
    color1.blue = 256 * 17;
    printf("%lu", color1.pixel);
    XAllocColor(dpy, cmap, &color1);
    XSetForeground(dpy, gc, color1.pixel);

    Pixmap pixmap = XCreatePixmap(dpy, w, WIDTH + 2, HIGHT + 2, 24);
    int i, j;
    unsigned short red, green, blue;
#pragma omp parallel for private(color1, i, j, red, green, blue)
    for (i = 0; i <= WIDTH; i++) {
        //i++;
        for (j = 0; j <= HIGHT; j++) {
            //j++;
            // color1.red = (int) (sqrt(abs((i - WIDTH / 2)) * abs((j - HIGHT / 2))) * 256) % (256 * 256);
            // color1.green = (int) (sqrt((i - WIDTH / 2) * (i - WIDTH / 2)) * 256) % (256 * 256);
            // color1.blue = (int) (sqrt((j - HIGHT / 2) * (j - HIGHT / 2)) * 256) % (256 * 256);
            red = i * j % 256;
            green = i % 256;
            blue = j % 256;
            red = green = blue = 0;
            // XAllocColor(dpy, cmap, &color1);
            XSetForeground(dpy, gc, red * 256 * 256 + green * 256 + blue);
            XDrawPoint(dpy, pixmap, gc, i, j);
        }
        XCopyArea(dpy, pixmap, w, gc, 0, 0, WIDTH, HIGHT, 0, 0);
    }
    XCopyArea(dpy, pixmap, w, gc, 0, 0, WIDTH, HIGHT, 0, 0);

//    while ();

    int k, m, center_x, center_y, counts;
    k = m = counts = 0;
#define CIRCLE_SIZE 40
    while (1) {
        center_x = e.xbutton.x;
        center_y = e.xbutton.y;
        srand(time(NULL));
        center_x = rand() % WIDTH;
        center_y = rand() % HIGHT;
        for (k = center_x - CIRCLE_SIZE; k < center_x + CIRCLE_SIZE; ++k) {
            k += 2;

            for (m = center_y - CIRCLE_SIZE; m < center_y + CIRCLE_SIZE; ++m) {
                m += 2;
                if (pow(abs(k - center_x), 2) + pow(abs(m - center_y), 2) > pow(CIRCLE_SIZE, 2))continue;
                red = k + m % 256;
                green = k % 256;
                blue = m % 256;
                XSetForeground(dpy, gc, red * 256 * 256 + green * 256 + blue);
                XDrawPoint(dpy, pixmap, gc, k, m);
            }
        }

        counts++;
        if (counts % 5 == 0) {
            printf("%d\n",counts);
            XCopyArea(dpy, pixmap, w, gc,
                      center_x - CIRCLE_SIZE, center_y - CIRCLE_SIZE,
                      CIRCLE_SIZE * 2, CIRCLE_SIZE * 2,
                      center_x - CIRCLE_SIZE, center_y - CIRCLE_SIZE);
        }
    }
    while (1) {
        XNextEvent(dpy, &e);
        if (e.type != 14) {
            printf("%d\n", e.type);
        }
        XSetBackground(dpy, gc, 0x000000);

        switch (e.type) {
            case ButtonPress:
                printf("x=%d, y=%d, button=%d\n", e.xbutton.x, e.xbutton.y, e.xbutton.button);
                if (e.xbutton.button == 3) {
                    XCloseDisplay(dpy);
                    return (0);
                }
                break;
            case ButtonRelease:
                //color1.red = 128;
                // color1.blue = (e.xbutton.x * 256) % (256 * 256);
                // color1.green = (e.xbutton.y * 256) % (256 * 256);
                // XAllocColor(dpy, cmap, &color1);
                // XSetForeground(dpy, gc, color1.pixel);




                break;
            case KeyPress:
                if (XLookupString((XKeyEvent *) &e, string, 9, &key, NULL) == 1) {
                    here[0] = string[0];
                    here[1] = '\0';
                    XDrawString(dpy, w, gc, 30 + l, 200, here, strlen(here));
                    l += 10;
                }
        }
        //XCopyArea(dpy, pixmap, w, gc, 0, 0, WIDTH, HIGHT, 0, 0);
    }
}

#pragma clang diagnostic pop
