/* xdraw.c */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define BORDER 2
#define WIDTH  350
#define HIGHT 250

char here[] = "I'm here!";

main(int argc, char **argv)
{
	Display *dpy;
	Window w;
	Window root;
	int    screen;
	unsigned long black, white;
	GC       gc;

	dpy = XOpenDisplay("");

	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);

	gc = XCreateGC(dpy, w, 0, NULL);

	XMapWindow(dpy, w);

	while(1){
	XDrawString(dpy, w, gc, WIDTH / 2, HIGHT / 2,
	here, strlen(here));
	XDrawPoint(dpy, w, gc,  30, 30);
	XDrawLine(dpy, w, gc,  33, 30, 300, 200);
	XDrawRectangle(dpy, w, gc,  43, 40, 100, 70);
	XDrawArc(dpy, w, gc,  100, 100, 30, 30, 0, 360*64);

	/* event loop */
	}
}

