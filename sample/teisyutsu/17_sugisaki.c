/* xdraw_e.c */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#define WIDTH  1000
#define HIGHT 800
#define BORDER 2

double f(double x, double x1, double y1, double x2, double y2);

int main(int argc,char **argv){
	Display *dpy;
	Window w;
	Window root;
	int    screen;
	unsigned long black, white;
	GC       gc;
    XEvent e;

	dpy = XOpenDisplay("");
	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);

	Colormap cmap;
	XColor c0, c1;
	cmap = DefaultColormap(dpy, 0);
	XAllocNamedColor(dpy, cmap, "cyan", &c1, &c0);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);
    XSelectInput(dpy, w, PointerMotionMask | ButtonPressMask | ButtonReleaseMask);
	XMapWindow(dpy, w);
	int buttonFlag = 0;
	int tmpX, tmpY;
	while(1){
	   XNextEvent(dpy, &e);
	   switch (e.type){
		  case ButtonPress :
		  if(e.xbutton.button == 1)buttonFlag = 1;
		  else if(e.xbutton.button == 3)XClearWindow(dpy, w);
		  tmpX = e.xbutton.x;
		  tmpY = e.xbutton.y;
		  break;
		  case ButtonRelease:
		  buttonFlag = 0;
		  break;
          case MotionNotify :
		  if(buttonFlag)
		  {
			XSetForeground(dpy, gc, c1.pixel);
			if(tmpX == e.xbutton.x){
				XDrawLine(dpy, w, gc, tmpX, 0, tmpX, 2000);
			}
			else{
				XDrawLine(dpy, w, gc, 0, f(0, tmpX, tmpY, e.xbutton.x, e.xbutton.y), 2000, f(2000, tmpX, tmpY, e.xbutton.x, e.xbutton.y));
			}
			XSetForeground(dpy, gc, black);
			XDrawLine(dpy, w, gc,tmpX, tmpY, e.xbutton.x, e.xbutton.y);
			tmpX = e.xbutton.x;
			tmpY = e.xbutton.y;
		  }
		  break;
	   }
	}
}

double f(double x, double x1, double y1, double x2, double y2){
	return (y1 - y2)/(x1 - x2) * (x-x1) + y1;
}
