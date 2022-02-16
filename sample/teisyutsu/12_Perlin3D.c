#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define PI 3.14159265

#define Perlin1 True    /*      　　　　　　 Perlin1←ー→Perlin5      */
#define Perlin2 True    /* ノイズの大きさ：     大      ←ー→     小         */
#define Perlin3 True    /* ※Trueで足し合わせる，Falseで足し合わせない  */
#define Perlin4 True
#define Perlin5 True

#define Elevation 50        //ノイズの振幅の倍率 (実数)
#define mapsize 128         //マップの大きさ (自然数)
#define extent 30           //描画距離 (自然数)
#define speed 0.5           //移動速度 (実数)
#define levitation False    //浮遊の有無 (Trueで浮遊，Falseで歩行)
#define polygon False       //ポリゴンの強調の有無 (Trueで枠線あり，Falseで枠線なし)
#define angle 90            //画角 (０〜１８０の実数)
#define xlight 0            //光源の位置 (実数)
#define ylight 100
#define zlight 100
#define xprimary 10         //プレイヤーの最初の位置　(実数)
#define yprimary 10
#define zprimary 10
#define sprimary 0          //プレイヤーが最初に向いている上下方向 (実数)
#define tprimary 45         //プレイヤーが最初に向いている左右方向 (実数)
#define details True        //プレイヤーの位置，向いている方向の表示の有無 (Trueで表示する，Falseで表示しない)

double xs(double x, double y, double z, double s, double t, int a, int b, int c) {
    return cos(t * PI / 180.0) * (a - x) - sin(t * PI / 180.0) * (c - z);
}

double ys(double x, double y, double z, double s, double t, int a, int b, int c) {
    return -sin(s * PI / 180.0) * sin(t * PI / 180.0) * (a - x) + cos(s * PI / 180.0) * (b - y) -
           sin(s * PI / 180.0) * cos(t * PI / 180.0) * (c - z);
}

double zs(double x, double y, double z, double s, double t, int a, int b, int c) {
    return cos(s * PI / 180.0) * sin(t * PI / 180.0) * (a - x) + sin(s * PI / 180.0) * (b - y) +
           cos(s * PI / 180.0) * cos(t * PI / 180.0) * (c - z);
}

double f(double x) {
    return 1 - 6 * pow(fabs(x), 5) + 15 * pow(fabs(x), 4) - 10 * pow(fabs(x), 3);
}

double p1(double x, double y, double a, double b) {
    return f(x) * f(y) * (-sin(b) * x / tan(a + PI / 4.0) + cos(b) * y / tan(a + PI / 4.0)) * (1 - fabs(x)) *
           (1 - fabs(y));
}

double
p2(double x, double y, double pa1[4][2], double pa2[9][2], double pa3[25][2], double pa4[81][2], double pa5[289][2]) {
    int i1, i2;
    double s = 0;
    if (Perlin1)
        for (i1 = 0; i1 <= 1; i1++)
            for (i2 = 0; i2 <= 1; i2++) {
                if (fabs(x / 800.0 - i1) <= 1 && fabs(y / 800.0 - i2) <= 1)
                    s += p1(x / 800.0 - i1, y / 800.0 - i2, (PI / 4.0) * pa1[i1 + i2 * 2][0],
                            PI * 2 * pa1[i1 + i2 * 2][1]) * 4;
            }
    if (Perlin2)
        for (i1 = 0; i1 <= 2; i1++)
            for (i2 = 0; i2 <= 2; i2++) {
                if (fabs(x / 400.0 - i1) <= 1 && fabs(y / 400.0 - i2) <= 1)
                    s += p1(x / 400.0 - i1, y / 400.0 - i2, (PI / 4.0) * pa2[i1 + i2 * 3][0],
                            PI * 2 * pa2[i1 + i2 * 3][1]) * 2;
            }
    if (Perlin3)
        for (i1 = 0; i1 <= 4; i1++)
            for (i2 = 0; i2 <= 4; i2++) {
                if (fabs(x / 200.0 - i1) <= 1 && fabs(y / 200.0 - i2) <= 1)
                    s += p1(x / 200.0 - i1, y / 200.0 - i2, (PI / 4.0) * pa3[i1 + i2 * 5][0],
                            PI * 2 * pa3[i1 + i2 * 5][1]) * 1;
            }
    if (Perlin4)
        for (i1 = 0; i1 <= 8; i1++)
            for (i2 = 0; i2 <= 8; i2++) {
                if (fabs(x / 100.0 - i1) <= 1 && fabs(y / 100.0 - i2) <= 1)
                    s += p1(x / 100.0 - i1, y / 100.0 - i2, (PI / 4.0) * pa4[i1 + i2 * 9][0],
                            PI * 2 * pa4[i1 + i2 * 9][1]) * 0.5;
            }
    if (Perlin5)
        for (i1 = 0; i1 <= 16; i1++)
            for (i2 = 0; i2 <= 16; i2++) {
                if (fabs(x / 50.0 - i1) <= 1 && fabs(y / 50.0 - i2) <= 1)
                    s += p1(x / 50.0 - i1, y / 50.0 - i2, (PI / 4.0) * pa5[i1 + i2 * 17][0],
                            PI * 2 * pa5[i1 + i2 * 17][1]) * 0.25;
            }
    return s;
}

int fe1(int e, int x) {
    if (x - e <= 0)return 0;
    if (x - e > 0)return x - e;
}

int fe2(int e, int x, int m) {
    if (x + e <= m - 1)return x + e;
    if (x + e > m - 1)return m - 1;
}

unsigned long MyColor(Display *dpy, int r, int g, int b) {
    Colormap cmap;
    XColor c0, c1;
    char color[13];
    sprintf(color, "rgb:%02x/%02x/%02x", r, g, b);
    cmap = DefaultColormap(dpy, 0);
    XAllocNamedColor(dpy, cmap, color, &c1, &c0);
    return c1.pixel;
}

void main(int argc, char **argv) {
    Display *dpy;
    Window w;
    Window root;
    int screen;
    unsigned long black, white;
    GC gc;
    XEvent e;

    dpy = XOpenDisplay("");
m    root = DefaultRootWindow (dpy);
    screen = DefaultScreen (dpy);
    white = WhitePixel (dpy, screen);
    black = BlackPixel (dpy, screen);

    Pixmap p1;

    w = XCreateSimpleWindow(dpy, root, 100, 100, 700, 700, 2, black, white);
    p1 = XCreatePixmap(dpy, w, 700, 700, DefaultDepth(dpy, 0));
    gc = XCreateGC(dpy, w, 0, NULL);
    XSelectInput(dpy, w, ButtonPressMask | ButtonReleaseMask | PointerMotionMask | KeyPressMask);
    XMapWindow(dpy, w);

    int xmo = 0, ymo = 0, i, i1, i2, i3, f = 0, f1 = 0, tt = 0, c;
    char str[20];
    double x = xprimary, y = yprimary, z = zprimary, s = sprimary, t = tprimary, xd1, yd1, zd1, xd2, yd2, zd2, xd3, yd3, zd3, xd4, yd4, zd4, ac, vy;
    double xl = xlight, yl = ylight, zl = zlight, ang = 350 / tan(angle * PI / 360.0);
    double map[mapsize + 1][mapsize + 1];
    unsigned long col[mapsize][mapsize][2];
    unsigned long blue, sun;

    double pa1[4][2], pa2[9][2], pa3[25][2], pa4[81][2], pa5[289][2];
    srand((unsigned int) time(NULL));

    if (Perlin1)for (i1 = 0; i1 <= 3; i1++)for (i2 = 0; i2 <= 1; i2++)pa1[i1][i2] = (rand() % 10001) / 10000.0;
    if (Perlin2)for (i1 = 0; i1 <= 8; i1++)for (i2 = 0; i2 <= 1; i2++)pa2[i1][i2] = (rand() % 10001) / 10000.0;
    if (Perlin3)for (i1 = 0; i1 <= 24; i1++)for (i2 = 0; i2 <= 1; i2++)pa3[i1][i2] = (rand() % 10001) / 10000.0;
    if (Perlin4)for (i1 = 0; i1 <= 80; i1++)for (i2 = 0; i2 <= 1; i2++)pa4[i1][i2] = (rand() % 10001) / 10000.0;
    if (Perlin5)for (i1 = 0; i1 <= 288; i1++)for (i2 = 0; i2 <= 1; i2++)pa5[i1][i2] = (rand() % 10001) / 10000.0;

    for (i1 = 0; i1 <= mapsize; i1++)
        for (i2 = 0; i2 <= mapsize; i2++)
            map[i1][i2] = p2(i1 * 800.0 / mapsize, i2 * 800.0 / mapsize, pa1, pa2, pa3, pa4, pa5) * Elevation;

    for (i1 = 0; i1 <= mapsize - 1; i1++)
        for (i2 = 0; i2 <= mapsize - 1; i2++) {
            col[i1][i2][0] = MyColor(dpy, 0, (255 / PI) * acos((xl * (map[i1 + 1][i2] - map[i1][i2]) - yl +
                                                                zl * (map[i1][i2 + 1] - map[i1][i2])) /
                                                               (sqrt(pow(xl, 2) + pow(yl, 2) + pow(zl, 2)) *
                                                                sqrt(pow(map[i1 + 1][i2] - map[i1][i2], 2) + 1 +
                                                                     pow(map[i1][i2 + 1] - map[i1][i2], 2)))), 0);
            col[i1][i2][1] = MyColor(dpy, 0, (255 / PI) * acos((xl * (map[i1 + 1][i2 + 1] - map[i1][i2 + 1]) - yl +
                                                                zl * (map[i1 + 1][i2 + 1] - map[i1 + 1][i2])) /
                                                               (sqrt(pow(xl, 2) + pow(yl, 2) + pow(zl, 2)) *
                                                                sqrt(pow(map[i1 + 1][i2 + 1] - map[i1][i2 + 1], 2) + 1 +
                                                                     pow(map[i1 + 1][i2 + 1] - map[i1 + 1][i2], 2)))),
                                     0);
        }
    blue = MyColor(dpy, 0, 0, 255);
    sun = MyColor(dpy, 255, 255, 200);

    XSync(dpy, True);

    while (1) {

        if (XEventsQueued(dpy, QueuedAfterReading)) {
            XNextEvent(dpy, &e);
            if (e.type == ButtonPress)f = 1;
            if (e.type == ButtonRelease)f = 0;
            if (f == 1 && f1 == 1) {
                if (s + (e.xbutton.y - ymo) * 0.3 >= -90 && s + (e.xbutton.y - ymo) * 0.3 <= 90)
                    s += (e.xbutton.y - ymo) * 0.3;
                t += (xmo - e.xbutton.x) * 0.3;
            }
            xmo = e.xbutton.x;
            ymo = e.xbutton.y;
            f1 = 1;

            if (e.xkey.keycode == 25) {
                if (x + sin(t * PI / 180.0) * speed >= 0 && x + sin(t * PI / 180.0) * speed <= mapsize)
                    x += sin(t * PI / 180.0) * speed;
                if (z + cos(t * PI / 180.0) * speed >= 0 && z + cos(t * PI / 180.0) * speed <= mapsize)
                    z += cos(t * PI / 180.0) * speed;
            }
            if (e.xkey.keycode == 38) {
                if (x - cos(t * PI / 180.0) * speed >= 0 && x - cos(t * PI / 180.0) * speed <= mapsize)
                    x += -cos(t * PI / 180.0) * speed;
                if (z + sin(t * PI / 180.0) * speed >= 0 && z + sin(t * PI / 180.0) * speed <= mapsize)
                    z += sin(t * PI / 180.0) * speed;
            }
            if (e.xkey.keycode == 39) {
                if (x - sin(t * PI / 180.0) * speed >= 0 && x - sin(t * PI / 180.0) * speed <= mapsize)
                    x -= sin(t * PI / 180.0) * speed;
                if (z - cos(t * PI / 180.0) * speed >= 0 && z - cos(t * PI / 180.0) * speed <= mapsize)
                    z -= cos(t * PI / 180.0) * speed;
            }
            if (e.xkey.keycode == 40) {
                if (x + cos(t * PI / 180.0) * speed >= 0 && x + cos(t * PI / 180.0) * speed <= mapsize)
                    x -= -cos(t * PI / 180.0) * speed;
                if (z - sin(t * PI / 180.0) * speed >= 0 && z - sin(t * PI / 180.0) * speed <= mapsize)
                    z -= sin(t * PI / 180.0) * speed;
            }

            if (levitation) {
                if (e.xkey.keycode == 65) { y++; }
                if (e.xkey.keycode == 50) { y--; }

            } else {
                i1 = (int) (x);
                i2 = (int) (z);
                if ((x - i1) + (z - i2) <= 1)
                    y = (x - i1) * (map[i1 + 1][i2] - map[i1][i2]) + (z - i2) * (map[i1][i2 + 1] - map[i1][i2]) +
                        map[i1][i2] + 7;
                if ((x - i1) + (z - i2) > 1)
                    y = (x - i1 - 1) * (map[i1 + 1][i2 + 1] - map[i1][i2 + 1]) +
                        (z - i2 - 1) * (map[i1 + 1][i2 + 1] - map[i1 + 1][i2]) + map[i1 + 1][i2 + 1] + 7;
            }

        }

        XSetForeground(dpy, gc, sun);
        if (zs(x, y, z, s, t, xl, yl, zl) > 1)
            XFillArc(dpy, p1, gc, 350 + xs(x, y, z, s, t, xl, yl, zl) / zs(x, y, z, s, t, xl, yl, zl) * 175 - 20, 350 -
                                                                                                                  ys(x,
                                                                                                                     y,
                                                                                                                     z,
                                                                                                                     s,
                                                                                                                     t,
                                                                                                                     xl,
                                                                                                                     yl,
                                                                                                                     zl) /
                                                                                                                  zs(x,
                                                                                                                     y,
                                                                                                                     z,
                                                                                                                     s,
                                                                                                                     t,
                                                                                                                     xl,
                                                                                                                     yl,
                                                                                                                     zl) *
                                                                                                                  175 -
                                                                                                                  20,
                     40, 40, 0, 360 * 64);


        for (i = extent; i >= 0; i--)
            for (i1 = fe1(extent, x); i1 <= fe2(extent, x, mapsize); i1++)
                for (i2 = fe1(extent, z); i2 <= fe2(extent, z, mapsize); i2++) {
                    xd1 = xs(x, y, z, s, t, i1, map[i1][i2], i2);
                    yd1 = ys(x, y, z, s, t, i1, map[i1][i2], i2);
                    zd1 = zs(x, y, z, s, t, i1, map[i1][i2], i2);
                    xd2 = xs(x, y, z, s, t, i1 + 1, map[i1 + 1][i2], i2);
                    yd2 = ys(x, y, z, s, t, i1 + 1, map[i1 + 1][i2], i2);
                    zd2 = zs(x, y, z, s, t, i1 + 1, map[i1 + 1][i2], i2);
                    xd3 = xs(x, y, z, s, t, i1, map[i1][i2 + 1], i2 + 1);
                    yd3 = ys(x, y, z, s, t, i1, map[i1][i2 + 1], i2 + 1);
                    zd3 = zs(x, y, z, s, t, i1, map[i1][i2 + 1], i2 + 1);
                    xd4 = xs(x, y, z, s, t, i1 + 1, map[i1 + 1][i2 + 1], i2 + 1);
                    yd4 = ys(x, y, z, s, t, i1 + 1, map[i1 + 1][i2 + 1], i2 + 1);
                    zd4 = zs(x, y, z, s, t, i1 + 1, map[i1 + 1][i2 + 1], i2 + 1);

                    if ((int) (sqrt((x - i1 - 0.3) * (x - i1 - 0.3) + (z - i2 - 0.3) * (z - i2 - 0.3)) + 0.5) == i &&
                        zd1 > 1 && zd2 > 1 && zd3 > 1) {
                        XSetForeground(dpy, gc, col[i1][i2][0]);
                        XPoint point[6] = {350 + xd1 / zd1 * ang, 350 - yd1 / zd1 * ang, 350 + xd2 / zd2 * ang,
                                           350 - yd2 / zd2 * ang, 350 + xd3 / zd3 * ang, 350 - yd3 / zd3 * ang};
                        XFillPolygon(dpy, p1, gc, point, 3, Complex, CoordModeOrigin);
                        if (polygon) {
                            XSetForeground(dpy, gc, black);
                            XDrawLine(dpy, p1, gc, 350 + xd1 / zd1 * ang, 350 - yd1 / zd1 * ang, 350 + xd2 / zd2 * ang,
                                      350 - yd2 / zd2 * ang);
                            XDrawLine(dpy, p1, gc, 350 + xd1 / zd1 * ang, 350 - yd1 / zd1 * ang, 350 + xd3 / zd3 * ang,
                                      350 - yd3 / zd3 * ang);
                            XDrawLine(dpy, p1, gc, 350 + xd2 / zd2 * ang, 350 - yd2 / zd2 * ang, 350 + xd3 / zd3 * ang,
                                      350 - yd3 / zd3 * ang);
                        }
                    }
                    if ((int) (sqrt((x - i1 - 0.7) * (x - i1 - 0.7) + (z - i2 - 0.7) * (z - i2 - 0.7)) + 0.5) == i &&
                        zd4 > 1 && zd2 > 1 && zd3 > 1) {
                        XSetForeground(dpy, gc, col[i1][i2][1]);
                        XPoint point[6] = {350 + xd4 / zd4 * ang, 350 - yd4 / zd4 * ang, 350 + xd2 / zd2 * ang,
                                           350 - yd2 / zd2 * ang, 350 + xd3 / zd3 * ang, 350 - yd3 / zd3 * ang};
                        XFillPolygon(dpy, p1, gc, point, 3, Complex, CoordModeOrigin);
                        if (polygon) {
                            XSetForeground(dpy, gc, black);
                            XDrawLine(dpy, p1, gc, 350 + xd4 / zd4 * ang, 350 - yd4 / zd4 * ang, 350 + xd2 / zd2 * ang,
                                      350 - yd2 / zd2 * ang);
                            XDrawLine(dpy, p1, gc, 350 + xd4 / zd4 * ang, 350 - yd4 / zd4 * ang, 350 + xd3 / zd3 * ang,
                                      350 - yd3 / zd3 * ang);
                            XDrawLine(dpy, p1, gc, 350 + xd2 / zd2 * ang, 350 - yd2 / zd2 * ang, 350 + xd3 / zd3 * ang,
                                      350 - yd3 / zd3 * ang);
                        }
                    }
                }

        if (details) {
            XSetForeground(dpy, gc, black);
            sprintf(str, "x:%f", x);
            XDrawString(dpy, p1, gc, 600, 400, str, strlen(str));
            sprintf(str, "y:%f", y);
            XDrawString(dpy, p1, gc, 600, 420, str, strlen(str));
            sprintf(str, "z:%f", z);
            XDrawString(dpy, p1, gc, 600, 440, str, strlen(str));
            sprintf(str, "s:%f", s);
            XDrawString(dpy, p1, gc, 600, 460, str, strlen(str));
            sprintf(str, "t:%f", t);
            XDrawString(dpy, p1, gc, 600, 480, str, strlen(str));
        }


        XCopyArea(dpy, p1, w, gc, 0, 0, 700, 700, 0, 0);
        XFlush(dpy);

        XSetForeground(dpy, gc, blue);
        XFillRectangle(dpy, p1, gc, 0, 0, 700, 700);

        if (XEventsQueued(dpy, QueuedAfterReading)) {
            XNextEvent(dpy, &e);
            if (e.type == ButtonPress)f = 1;
            if (e.type == ButtonRelease)f = 0;
        }
        XSync(dpy, True);
        while (!(XEventsQueued(dpy, QueuedAfterReading)));

    }

}

