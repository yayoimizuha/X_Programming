/* orbit.c */
#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <string.h>

#define BORDER 2
#define WIDTH 1200
#define HIGHT 900
int  main(int argc, char **argv){
	Display *dpy;
	Window w, quit, ecc, day;
	Window temp = ecc;
 	Window root;
 	int screen;
 	unsigned long black, white;
 	GC gc;
 	XEvent e;
 	Colormap cmap;
	XColor c0, c1;
	int i=0;
	int t;
	char days[256];
	char ec[256];
	char moji[256];
	char da[256];
	//gravitational constant
	float k;
	k = 7.496;
	//sun data
	unsigned long r1;
	r1 = 50;
	//earth data
	long r2;
	float e1;
	r2 = 30/*6.372*/;
	//earth sun distance
	int a,b,x,y,d;
	float a1=1;
	dpy = XOpenDisplay("");

	root = DefaultRootWindow(dpy);
	screen = DefaultScreen(dpy);
	white = WhitePixel(dpy, screen);
	black = BlackPixel(dpy, screen);
	cmap = DefaultColormap(dpy, 0);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, black);
/*　サブウインドウを作る　*/
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, black);
	ecc = XCreateSimpleWindow(dpy, w, WIDTH-110, 0, 110, 70, BORDER, white, black);
	day = XCreateSimpleWindow(dpy, w, WIDTH-110, 70, 110, 70, BORDER, white, black);
	gc = XCreateGC(dpy, w, 0, NULL);

	XSelectInput(dpy, w, ButtonPressMask|KeyPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);	/*　サブウインドウ用　*/
	XSelectInput(dpy, ecc, ButtonPressMask|KeyPressMask|KeyReleaseMask);
	XSelectInput(dpy, day, ButtonPressMask);
	
	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);	/*　サブウインドウも表示　*/
	
	XSetForeground(dpy, gc, white);
	XFillRectangle(dpy,w,gc,0,0,WIDTH,HIGHT);
	printf("離心率(0~1): ");
	scanf("%f",&e1);
	while(1){
		if(XEventsQueued(dpy, QueuedAfterReading)){ /* for ground */
			XNextEvent(dpy, &e);
			switch(e.type){
				case ButtonPress :
					if(e.xany.window == quit){
						return 0;
						i=0;
						XClearWindow(dpy, w);
					}
					else if(e.xany.window == ecc){
						temp = ecc;
					}
				case KeyPress:
					t = e.xkey.keycode;
					if(t>9 && t<19){
						i=0;
						a1 = t-9;
						sprintf(ec,"%f",a1);
						XSetForeground(dpy, gc, white);
						XClearWindow(dpy, ecc);
						XClearWindow(dpy, day);
						XDrawString(dpy, ecc, gc, 10, 40, ec, strlen(ec));
					}
				case KeyRelease:
					e1 == 0;
					
			}
		}else{	/* back ground */
			a=a1*80;
			b=sqrt((a*a)-(a*a*e1*e1));
			XAllocNamedColor(dpy,cmap,"orange",&c1,&c0);
	    XSetForeground(dpy,gc,c1.pixel);
			XFillArc(dpy, w, gc, (WIDTH/2)-(r1/2)+((a*e1)/2), (HIGHT/2)-(r1/2),r1,r1,0,360*64);
			XSetForeground(dpy,gc,white);
			XDrawString(dpy, quit, gc, 4, 10, "Exit", 4);
			XDrawString(dpy, ecc, gc, 10, 10, "Type a number to", 16);
			XDrawString(dpy, ecc, gc, 10, 20, "change the semi-", 16);
			XDrawString(dpy, ecc, gc, 10, 30, "major axis(1~7)", 15);
			d=sqrt(a1*a1*a1*pow(10,6)/k);
			sprintf(da,"%d",d);
			XDrawString(dpy, day, gc, 10, 15, "Orbital period :", 16);
			XDrawString(dpy, day, gc, 10, 30, da, strlen(da));
			sprintf(days,"%d",i);
			XDrawString(dpy, w, gc, (WIDTH/2)-70, 15, "current day", 11);
			XDrawString(dpy, w, gc, WIDTH/2, 15, days, strlen(days));
			x=(WIDTH)/2+a*cos(i*3.14/(d/2));
			y=(HIGHT)/2+b*sin(i*3.14/(d/2));
			XAllocNamedColor(dpy,cmap,"blue",&c1,&c0);
	    XSetForeground(dpy,gc,c1.pixel);
			XFillArc(dpy, w, gc,x-(r2/2),y-(r2/2),r2,r2,0,360*64);
			i++;
			if(i==d) i=0;
			XFlush(dpy);
			XClearWindow(dpy, w);
			usleep(5000);
		}
	}
}
