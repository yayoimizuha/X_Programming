/* コムソート */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>

#define BORDER 2
#define WIDTH  1200
#define HIGHT 900

int main(int argc,char **argv)
{
	Display *dpy;
	Window w;
	Window root;
	int    screen;
	unsigned long black, white;
	GC  gc;
	XEvent e;
	dpy = XOpenDisplay("");
	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);
	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);
	
	XSelectInput(dpy, w, ButtonPressMask);
	XMapWindow(dpy, w);
	XSetForeground(dpy, gc, black);
int size,p,x,h,a,i,c,b;
static int datab[100000000];
static int datac[100000000];
size=250;
c=1;
while(1){
srand((unsigned int)time(NULL));
for(p=0;p<size;p++){
	datab[p]=rand()%400+1;
}
for(p=0;p<size;p++){
	datac[p]=datab[p];
	}
	c=1;  // ここまで乱数関係
	while(c=1){  //ループ開始
		h=size;
		h=h*1.3;
			while(h!=1){
				h=h/1.3;
				for(i=0; i<size-h;i++){
					if(datac[i]<datac[i+h]){
						a=datac[i];
						datac[i]=datac[i+h];
						datac[i+h]=a;
					}
					}
					for(x=0;x<size+100;x++){  
						XDrawLine(dpy, w, gc,  10+4*x, 850, 10+4*x,850-datac[x]);
						XDrawLine(dpy, w, gc,  11+4*x, 850, 11+4*x,850-datac[x]);
						XDrawLine(dpy, w, gc,  12+4*x, 850, 12+4*x,850-datac[x]);
						XDrawLine(dpy, w, gc,  13+4*x, 850, 13+4*x,850-datac[x]);
						XDrawLine(dpy, w, gc,  10+4*x, 400, 10+4*x, 400-datab[x]);
						XDrawLine(dpy, w, gc,  11+4*x, 400, 11+4*x, 400-datab[x]);
						XDrawLine(dpy, w, gc,  12+4*x, 400, 12+4*x, 400-datab[x]);
						XDrawLine(dpy, w, gc,  13+4*x, 400, 13+4*x, 400-datab[x]);
						XDrawLine(dpy, w, gc,  0, 300, 1000, 300);
						XDrawLine(dpy, w, gc,  0, 200, 1000, 200);
						XDrawLine(dpy, w, gc,  0, 100, 1000, 100);
						XDrawLine(dpy, w, gc,  0, 750, 1000, 750);
						XDrawLine(dpy, w, gc,  0, 650, 1000, 650);
						XDrawLine(dpy, w, gc,  0, 550, 1000, 550);
						XDrawLine(dpy, w, gc,  0, 450, 1000, 450);
					}
					usleep(300000);
					XFlush(dpy);
			XClearWindow(dpy,w);
			srand((unsigned int)time(NULL));
			}
		}
	}
}  //ループ終了
