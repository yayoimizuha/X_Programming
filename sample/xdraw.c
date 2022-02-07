/* xdraw.c */
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>

#define BORDER 2
#define WIDTH  350
#define HIGHT 250

int main(int argc, char **argv) {
    Display *dpy;
    Window w;
    Window root;
    int screen;
    unsigned long black, white;
    GC gc;
    char here[] = "I'm here!";
    char moji[256];            /* 日本語メッセージが入る場所 */

    XFontSet fs;             /* XFontsetで日本語フォントを選ぶ（日） */
    char **miss, *def;
    int n_miss;              /* 変数として宣言しておく（日） */

    Colormap cmap;             /* カラーマップ構造体 */
    XColor c0, c1, test1;              /* カラー構造体 */

    setlocale(LC_ALL, "");        /* 1st. lacaleをセットして（日） */

    dpy = XOpenDisplay("");

    root = DefaultRootWindow (dpy);
    screen = DefaultScreen (dpy);
    white = WhitePixel (dpy, screen);
    black = BlackPixel (dpy, screen);
    cmap = DefaultColormap(dpy, 0);  /* カラーマップを得る */
    XAllocNamedColor(dpy, cmap, "green", &c1, &c0);  /* c1.pixel */

    test1.red = 256 * 182;
    test1.green = 256 * 105;
    test1.blue = 256 * 255;
    XAllocColor(dpy, cmap, &test1);

    w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, white, black);

    gc = XCreateGC(dpy, w, 0, NULL);

    fs = XCreateFontSet(dpy, "-*-*-medium-r-normal-*-16-*", &miss, &n_miss, &def);
    /* 少なくともdpyに値がはいってからでないと駄目（日） */

    XMapWindow(dpy, w);
    int red, green, blue;
    red = green = blue = 128;
    printf("aaaaaaaaaaa");
    printf("aa %d", w);
    float x = 100;
    while (1) {
        XSetForeground(dpy, gc, white);  /* blackにする */
        XDrawString(dpy, w, gc, WIDTH / 2, HIGHT / 2, here, strlen(here));

        sprintf(moji, "漢字の例");       /* 画面に出す文字列を変数の中に入れる */
        XmbDrawString(dpy, w, fs, gc, 20, 40, moji, strlen(moji));   /* 漢字表示 */
        XDrawPoint(dpy, w, gc, 30, 30);

        XSetForeground(dpy, gc, c1.pixel);  /* greenにする */

        XDrawLine(dpy, w, gc, 33, 30, 300, 200);
        XDrawRectangle(dpy, w, gc, 43, 40, 100, 70);
        XSetForeground(dpy, gc, test1.pixel);

        XDrawArc(dpy, w, gc, 100, 100, 30, 30, 0, 360 * 64);
        //x += 0.0001;
        //XClearArea(dpy, w, (int) x - 1, 100 - 1, 30 + 2, 30 + 2, False);
        if (x > WIDTH) {
            //  x = 20;
        }

        test1.red = (256 * red) % (256 ^ 2);
        test1.green = (256 * green) % (256 ^ 2);
        test1.blue = (256 * blue) % (256 ^ 2);
        red++;
        //green += 2;
        //blue += 3;
        XAllocColor(dpy, cmap, &test1);
        XSetForeground(dpy, gc, test1.pixel);
        /* event loop */
    }
}

