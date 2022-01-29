/* 19_ .c */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#define BORDER 2
#define WIDTH 600
#define HIGHT 600

int main(int argc, char **argv)
{
	Display *dpy;
	Window w, quit, ap, am, bp, bm;
	Window root;
	int screen;
	unsigned long black, white;
	GC gc;
	XEvent e;
	int x, y, a = 10, b = 10;
	float i = 0;
	char A[256], B[256], siki[256];

	dpy = XOpenDisplay("");

	root = DefaultRootWindow(dpy);
	screen = DefaultScreen(dpy);
	white = WhitePixel(dpy, screen);
	black = BlackPixel(dpy, screen);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	ap = XCreateSimpleWindow(dpy, w, 400, 3, 12, 12, BORDER, black, white);
	am = XCreateSimpleWindow(dpy, w, 440, 3, 12, 12, BORDER, black, white);
	bp = XCreateSimpleWindow(dpy, w, 510, 3, 12, 12, BORDER, black, white);
	bm = XCreateSimpleWindow(dpy, w, 550, 3, 12, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);

	XSelectInput(dpy, w, ButtonPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);
	XSelectInput(dpy, ap, ButtonPressMask);
	XSelectInput(dpy, am, ButtonPressMask);
	XSelectInput(dpy, bp, ButtonPressMask);
	XSelectInput(dpy, bm, ButtonPressMask);

	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);

	XSetForeground(dpy, gc, black);
	XMapWindow(dpy, w);

	while (1)
	{
		if (XEventsQueued(dpy, QueuedAfterReading))
		{ /* for ground */
			XNextEvent(dpy, &e);
			switch (e.type)
			{
			case ButtonPress:
				if (e.xany.window == quit)
					return 0;
				if (e.xany.window == ap)
					a++;
				if (e.xany.window == am)
					a--;
				if (e.xany.window == bp)
					b++;
				if (e.xany.window == bm)
					b--;
				i = 0;
				XClearWindow(dpy, w);
			}
		}
		else
		{ /* back ground */
			static int flag = 0;
			XSetForeground(dpy, gc, black);
			XDrawString(dpy, quit, gc, 4, 10, "Exit", 4); /* Exit */
			
			XDrawString(dpy, ap, gc, 4, 10, "+", 4);			/* a+ */
			XDrawString(dpy, am, gc, 4, 10, "-", 4);			/* a- */
			XDrawString(dpy, bp, gc, 4, 10, "+", 4);			/* b+ */
			XDrawString(dpy, bm, gc, 4, 10, "-", 4);			/* b- */

			XDrawLine(dpy, w, gc, 50, 300, 550, 300); /* x軸 */
			XDrawLine(dpy, w, gc, 550, 300, 540, 290);
			XDrawLine(dpy, w, gc, 550, 300, 540, 310);
			
			XDrawLine(dpy, w, gc, 300, 50, 300, 550); /* y軸 */
			XDrawLine(dpy, w, gc, 300, 50, 290, 60);
			XDrawLine(dpy, w, gc, 300, 50, 310, 60);
			
			sprintf(siki,"x=%d*rad*sin(rad) y=%d*rad*cos(rad)", a, b); /* 式 */
			XDrawString(dpy, w, gc, 125, 15, siki, strlen(siki)); 
			
			sprintf(A,"a"); /* A */
			XDrawString(dpy, w, gc, 425, 15, A, strlen(A)); 
			
			sprintf(B,"b"); /* B */
			XDrawString(dpy, w, gc, 535, 15, B, strlen(B)); 
			
			if (flag)
				XSetForeground(dpy, gc, white);
			else
				XSetForeground(dpy, gc, black);
			XDrawPoint(dpy, w, gc, a * (i / 100) * sin(i / 100) + 300, b * (i / 100) * cos(i / 100) + 300); /* 点 */
			if (flag)
				i -= 0.1;
			else
				i += 0.1;
			if (i < 0)
				flag = 0;
			if (a*(i/100)*sin(i/100)+300 >550 || a*(i/100)*sin(i/100)+300 <50 || b*(i/100)*cos(i/100)+300 >550 || a*(i/100)*cos(i/100)+300 <50)
				flag = 1;
			usleep(1);
			XFlush(dpy);
		}
	}
}
