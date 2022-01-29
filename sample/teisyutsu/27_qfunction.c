#include <stdio.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>

#define BORDER 2
#define WIDTH 500
#define HIGHT 350

int  main(int argc, char **argv){
	Display *dpy;
	Window w, quit,reset;
 	Window root;
 	int screen;
 	double xx1,yy1,xx2,yy2,xx3,yy3;
 	int count=0;
 	double a,b,c,i;
 	unsigned long black, white;
 	GC gc;
 	XEvent e;
	int  x, y;
	char moji[100];
	char numa[100];
	char numb[100];
	char numc[100];

	dpy = XOpenDisplay("");

	root = DefaultRootWindow(dpy);
	screen = DefaultScreen(dpy);
	white = WhitePixel(dpy, screen);
	black = BlackPixel(dpy, screen);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
/*　サブウインドウを作る　*/
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	reset = XCreateSimpleWindow(dpy, w, 55, 3, 35, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);

	XSelectInput(dpy, w, ButtonPressMask | ButtonReleaseMask | KeyPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);
	XSelectInput(dpy, reset, ButtonPressMask);	/*　サブウインドウ用　*/

	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);	/*　サブウインドウも表示　*/

	XSetForeground(dpy, gc, black);
	while(1){
		if(XEventsQueued(dpy, QueuedAfterReading)){ /* for ground */
			XNextEvent(dpy, &e);
			switch(e.type){
				case ButtonPress :
					if(e.xany.window == quit){
						return 0;
					}else if(e.xany.window == reset){
						XClearWindow(dpy, w);
						count=0;
					}
					break;
				case ButtonRelease :
					if(e.xany.window == w){
						if(count == 0){
							xx1=e.xbutton.x;
							yy1=e.xbutton.y;
							XDrawPoint(dpy,w,gc,xx1,yy1);
							count++;
						}else if(count == 1){
							xx2=e.xbutton.x;
							yy2=e.xbutton.y;
							XDrawPoint(dpy,w,gc,xx2,yy2);
							count++;
						}else if(count == 2){
							xx3=e.xbutton.x;
							yy3=e.xbutton.y;
							XDrawPoint(dpy,w,gc,xx3,yy3);
							count++;
							XDrawString(dpy, w, gc, 100, 15, "perss enter to draw graph", 25);
							xx1-=250;
							yy1-=175;
							xx2-=250;
							yy2-=175;
							xx3-=250;
							yy3-=175;
							yy1*=-1;
							yy2*=-1;
							yy3*=-1;
							a=((yy1-yy2)*(xx1-xx3)-(yy1-yy3)*(xx1-xx2))/((xx1-xx2)*(xx1-xx3)*(xx2-xx3));
							b=(yy1-yy2)/(xx1-xx2)-a*(xx1+xx2);
							c=yy1-a*xx1*xx1-b*xx1;
						}
					}
					break;
				case KeyPress :
					if(count == 3 && e.xkey.keycode == 36){
						sprintf(moji,"a=%3.3f b=%3.3f c=%3.3f",a,b,c);
						XDrawString(dpy,w,gc,4,40,moji,strlen(moji));
						for(i=-250;i<471;i+=0.001){
							XDrawPoint(dpy, w, gc, 280+i,175-(a*(30+i)*(30+i)+b*(30+i)+c));
							i+=0.01;
					}
					XFlush(dpy);
				}
			}
		}else{	/* back ground */
			XSetForeground(dpy,gc,black);
			XDrawString(dpy, quit, gc, 4, 10, "Exit", 4);
			XDrawString(dpy, reset, gc, 4, 10, "Reset", 5);
			XDrawLine(dpy, w, gc, 30, 175, 470, 175);	/* x軸 */
			XDrawLine(dpy, w, gc, 250, 30, 250, 320);		/* y軸 */
		}
	}
}
