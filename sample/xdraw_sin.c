/* xdraw_sin.c */
#include <stdio.h>
#include <math.h>    /* use math lib*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define BORDER 2
#define WIDTH  500
#define HIGHT 350

int main(int argc, char **argv)
{
	Display *dpy;
	Window w,quit;
	Window root;
	int    screen;
	unsigned long black, white;
	GC       gc;
	XEvent   e;
	int  i,x,y;

	dpy = XOpenDisplay("");

	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	/* Make Sub Window */
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);

	XSelectInput(dpy, w, ButtonPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);

	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);

	XSetForeground(dpy,gc,black);
	while(1){
	/* event loop */
	   XNextEvent(dpy, &e);
	   switch(e.type){
	   case ButtonPress : if(e.xany.window == quit)return ;
	      XDrawString(dpy,quit,gc,4,10,"Exit",4);
	      XDrawLine(dpy, w, gc, 50,175,450,175);
	      XDrawLine(dpy, w, gc, 50,50,50,300);
              for(i=0;i<400;i++){
	         XDrawPoint(dpy, w, gc, 50+i,175-100*sin(i/100.0));
	      }
	   }
	}
}

