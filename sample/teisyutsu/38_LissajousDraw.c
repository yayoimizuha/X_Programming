/* X_Lissajous.c */
#include <stdio.h>
#include <math.h>    /* use math lib*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>

#define BORDER 2
#define WIDTH  500
#define HIGHT 350
#define pi 3.141592

int main(int argc, char **argv)
{
	Display *dpy;
	Window w,quit,rotate,draw,reset;
	Window root;
	int screen;
	unsigned long black, white;
	GC gc;
	XEvent e;
	Colormap cmap;
	XColor c0, c1;
	int a,b,f,i,j,x,y,x1,y1,x2,y2;

	dpy = XOpenDisplay("");

	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);
	cmap = DefaultColormap(dpy, 0);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	/* Make Sub Window */
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	draw = XCreateSimpleWindow(dpy, w, 50, 3, 30, 12, BORDER, black, white);
	rotate = XCreateSimpleWindow(dpy, w, 90, 3, 40, 12, BORDER, black, white);
	reset = XCreateSimpleWindow(dpy, w, 140, 3, 35, 12, BORDER, black, white);

	gc = XCreateGC(dpy, w, 0, NULL);

	XSelectInput(dpy, w, ButtonPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);
	XSelectInput(dpy, draw, ButtonPressMask);
	XSelectInput(dpy, rotate, ButtonPressMask);
	XSelectInput(dpy, reset, ButtonPressMask);

	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);
	XSetForeground(dpy,gc,black);
	printf("整数aを入力してください。\n");
	scanf("%d",&a);
	printf("整数bを入力してください。\n");
	scanf("%d",&b);

	while(1){
		XNextEvent(dpy, &e);
		switch(e.type){
			case ButtonPress :
				i=0;
				XSetForeground(dpy,gc,black);
				XDrawString(dpy,quit,gc,4,10,"Exit",4);
				XDrawString(dpy,draw,gc,4,10,"Draw",4);
				XDrawString(dpy,rotate,gc,3,10,"Rotate",6);
				XDrawString(dpy,reset,gc,4,10,"Reset",5);
				XDrawLine(dpy, w, gc, 0,175,500,175);
				XDrawLine(dpy, w, gc, 250,0,250,350);
				
			if(e.xany.window == reset){
				XSetForeground(dpy,gc,white);
				XFillRectangle(dpy, w, gc, 0, 0, 500, 350);
				XSetForeground(dpy,gc,black);
				XDrawString(dpy,quit,gc,4,10,"Exit",4);
				XDrawString(dpy,draw,gc,4,10,"Draw",4);
				XDrawString(dpy,rotate,gc,3,10,"Rotate",6);
				XDrawString(dpy,reset,gc,4,10,"Reset",5);
				XDrawLine(dpy, w, gc, 0,175,500,175);
				XDrawLine(dpy, w, gc, 250,0,250,350);
				f=0;
			}
			
			if(e.xany.window == quit) return 0;
			
			if(e.xany.window == draw && f == 0){
				XSetForeground(dpy,gc,white);
				XFillRectangle(dpy, w, gc, 0, 0, 500, 350);
				XSetForeground(dpy,gc,black);
				XDrawString(dpy,quit,gc,4,10,"Exit",4);
				XDrawString(dpy,draw,gc,4,10,"Draw",4);
				XDrawString(dpy,rotate,gc,3,10,"Rotate",6);
				XDrawString(dpy,reset,gc,4,10,"Reset",5);
				XDrawLine(dpy, w, gc, 0,175,500,175);
				XDrawLine(dpy, w, gc, 250,0,250,350);
				for(i=0;i<1000*2*pi;i++){
					XDrawPoint(dpy, w, gc,250+100*cos(a*i/1000.0),175+100*sin(b*i/1000.0));
					usleep(100);
					XFlush(dpy);
				}
				f=1;
			}
			
			if(e.xany.window == rotate && f == 1){
				for(j=0;j<=36;j++){
					XSetForeground(dpy,gc,white);
					XFillRectangle(dpy, w, gc, 0, 0, 500, 350);
					XSetForeground(dpy,gc,black);
					XDrawString(dpy,quit,gc,4,10,"Exit",4);
					XDrawString(dpy,draw,gc,4,10,"Draw",4);
					XDrawString(dpy,rotate,gc,3,10,"Rotate",6);
					XDrawString(dpy,reset,gc,4,10,"Reset",5);
					XDrawLine(dpy, w, gc, 0,175,500,175);
					XDrawLine(dpy, w, gc, 250,0,250,350);
					for(i=0;i<1600*2*pi;i++){
						x1=250+100*cos(a*i/200.0);
						y1=175+100*sin(b*i/200.0+pi*j/18);
						x2=250+100*cos(a*(i+2)/200.0);
						y2=175+100*sin(b*(i+2)/200.0+pi*j/18);
						XDrawLine(dpy, w, gc,x1,y1,x2,y2);
						XFlush(dpy);
					}
				}
			}
		}
	}
}
