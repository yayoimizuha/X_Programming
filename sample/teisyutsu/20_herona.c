/* xdraw.c */
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <unistd.h>

#define BORDER 2
#define WIDTH  1200
#define HIGHT 1000

void nagasa(int *x,int *y,float *st);
int main(int argc,char **argv)
{
	Display *dpy;
	Window w,quit,clear;
	Window root;
	int    screen,x[3],y[3],flag=0,i;
	float  st[3],s,S,sq,gx,gy;
	char	 num[256],G[256];
	unsigned long black, white;
	GC       gc;
        XEvent e;

	dpy = XOpenDisplay("");
	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	clear = XCreateSimpleWindow(dpy, w, 50, 3, 35, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);
  XSelectInput(dpy,w,ButtonPressMask | ButtonReleaseMask );
  XSelectInput(dpy,quit,ButtonPressMask);
  XSelectInput(dpy,clear,ButtonPressMask);

	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);

	XMapWindow(dpy, w);
	XSetForeground(dpy, gc, black);
	while(1){
	 if(XEventsQueued(dpy, QueuedAfterReading)){
	   XNextEvent(dpy,&e);
	   switch (e.type){
			case ButtonPress :
				if(e.xany.window == quit)return 0;
				if(e.xany.window == clear){
				XClearWindow(dpy,w);
				flag=0;
				for(int a=0;a<3;a++){
					x[a]=0;
					y[a]=0;
					}
				}
				
				break;
				
			case ButtonRelease :
				if(flag==0){
					x[flag]=e.xbutton.x;
					y[flag]=e.xbutton.y;
					flag++;
					XDrawPoint(dpy, w, gc, x[0], y[0]);
					sprintf(num,"x=%d,y=%d",x[0],y[0]);
					XDrawString(dpy, w, gc, 10, 30, num, strlen(num));
				}
				else if(flag==1){
					x[flag]=e.xbutton.x;
					y[flag]=e.xbutton.y;
					flag++;
					XDrawPoint(dpy, w, gc, x[1], y[1]);
					XDrawLine(dpy, w, gc, x[0], y[0], x[1], y[1]);
					sprintf(num,"x=%d,y=%d",x[1],y[1]);
					XDrawString(dpy, w, gc, 10, 40, num, strlen(num));
				}
				else if(flag==2){
					x[flag]=e.xbutton.x;
					y[flag]=e.xbutton.y;
					XDrawLine(dpy, w, gc, x[0], y[0], x[2], y[2]);
					XDrawLine(dpy, w, gc, x[2], y[2], x[1], y[1]);
					flag++;
					sprintf(num,"x=%d,y=%d",x[2],y[2]);
					XDrawString(dpy, w, gc, 10, 50, num, strlen(num));
					
					nagasa(x,y,st);
				
					s=(st[0]+st[1]+st[2])/2;
					S=s*(s-st[0])*(s-st[1])*(s-st[2]);
					sq=sqrt(S);
					sprintf(num,"S=%f",sq);
					XDrawString(dpy, w, gc, 10, 70, num, strlen(num));
					
					gx=(x[0]+x[1]+x[2])/3;
					gy=(y[0]+y[1]+y[2])/3;
					sprintf(G,"G=(%f,%f)",gx,gy);
					XDrawString(dpy, w, gc, 10, 80, G, strlen(G));
					XFillArc(dpy, w, gc, gx+2, gy-2, 4, 4, 0, 360*64);
					
				}
				else if(flag==3)break;
				
				break;	
		}	
		
	}else{XDrawString(dpy, quit, gc, 4, 10, "Exit", 4);
				XDrawString(dpy, clear, gc, 4, 10, "clear", 5);
		
				usleep(16666);
				XFlush(dpy);
		}
	}
}

void nagasa(int *x,int *y,float *st)
{
	float a[3];
	int i;
	
	a[0]=((x[1]-x[0])*(x[1]-x[0]))+((y[1]-y[0])*(y[1]-y[0]));
	a[1]=((x[2]-x[1])*(x[2]-x[1]))+((y[2]-y[1])*(y[2]-y[1]));
	a[2]=((x[0]-x[2])*(x[0]-x[2]))+((y[0]-y[2])*(y[0]-y[2]));
	
	for(i=0; i<3; i++){
	*(st+i)=sqrt(a[i]);
	}
}
