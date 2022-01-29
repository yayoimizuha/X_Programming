/* xsin_m.c */
#include <stdio.h>
#include <math.h>	/*　数学関数を使う　*/
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <string.h>

#define BORDER 2
#define WIDTH 500
#define HIGHT 500

int  main(int argc, char **argv){
	Display *dpy;
	Window w, quit;
 	Window root;
 	int screen;
 	unsigned long black, white;
 	GC gc;
 	XEvent e;
	int i, x, y,k,j;
	k=0;
	j=0;
	float xx[3];
	float yy[3];
	float a,b,c;
	char keisuu[27];
	dpy = XOpenDisplay("");

	root = DefaultRootWindow(dpy);
	screen = DefaultScreen(dpy);
	white = WhitePixel(dpy, screen);
	black = BlackPixel(dpy, screen);

	w = XCreateSimpleWindow(dpy, root, -100, 100, WIDTH, HIGHT, BORDER, black, white);
/*　サブウインドウを作る　*/
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);

	XSelectInput(dpy, w, ButtonPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);	/*　サブウインドウ用　*/

	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);	/*　サブウインドウも表示　*/

	XSetForeground(dpy, gc, black);
	while(1){
		if(XEventsQueued(dpy, QueuedAfterReading)){ /* for ground */
			XNextEvent(dpy, &e);
			switch(e.type){
				case ButtonPress :
					if(e.xany.window == quit)return 0;
					xx[j] = e.xbutton.x;
		      		yy[j] = e.xbutton.y;
		 			j++;
		 			if(j==3){
		 			j=0;
		 			a=((yy[0]-yy[1])*(xx[0]-xx[2])-(yy[0]-yy[2])*(xx[0]-xx[1]))/((xx[0]-xx[1])*(xx[0]-xx[2])*(xx[1]-xx[2]));
	 	 			b=(yy[0]-yy[1])/(xx[0]-xx[1])-a*(xx[0]+xx[1]);
	 	 			c=yy[0]-a*xx[0]*xx[0]-b*xx[0];
					sprintf(keisuu, "y=%2.3fx^2+%2.3fx+%2.3f",a,b,c);
	 	 			XClearWindow(dpy, w);
			}
			}
		}else{	/* back ground */
			XDrawString(dpy, quit, gc, 4, 10, "Exit", 4);
			XDrawLine(dpy, w, gc, 0, HIGHT / 2, WIDTH, HIGHT / 2);	/* x軸 */
			XDrawLine(dpy, w, gc, WIDTH / 2, 0, WIDTH / 2, HIGHT);		/* y軸 */
			XDrawString(dpy, w, gc, WIDTH / 1.8, HIGHT / 1.5, keisuu, strlen(keisuu));
 			XDrawPoint(dpy, w, gc, k, a*k*k+b*k+c);
			if(k++>700)k=0;
			usleep(30);
			XFlush(dpy);
		}
	}
}

