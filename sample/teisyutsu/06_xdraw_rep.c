/* xdraw_sub.c */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>
#include <string.h>
#include <math.h>
#define BORDER 2
#define WIDTH  500
#define HIGHT 350

int main(int argc, char **argv)
{
	Display *dpy;
	Window w,quit;
	Window root;
	int    screen,i,n,x,ny=1,nd=1;
	static int flug=0;
	float s=1,ku=1,si=3,yc=0;
	unsigned long black, white;
	GC       gc;
	XEvent  e;
	dpy = XOpenDisplay("");




	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);
	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);
	XSelectInput(dpy,w,ButtonPressMask|  KeyPressMask);
	XSelectInput(dpy,quit,ButtonPressMask);

	XMapWindow(dpy, w);
	XMapSubwindows(dpy,w);

        XSetForeground(dpy,gc,black);
 	while(1){
		
	   if(XEventsQueued(dpy,QueuedAfterReading)){
			XNextEvent(dpy,&e);
			switch(e.type){
		case KeyPress :
					printf("kaycode=%d \n", e.xkey.keycode);
					i=0;
					si=170;
					switch (e.xkey.keycode){
						case 25 :flug=0;XClearWindow(dpy,w);break;
						case 24 :flug=1;XClearWindow(dpy,w);break;
						case 26 :flug=2;XClearWindow(dpy,w);break;
						case 27 :flug=3;XClearWindow(dpy,w);	break;
						case 28 :XClearWindow(dpy,w);	break;
						case 29 :ku=ku+0.25;XClearWindow(dpy,w);	break;
						case 30 :ku=ku-0.25;XClearWindow(dpy,w);	break;
						case 31 :yc=0;ku=1;s=1;i=0;nd=1;ny=1;XClearWindow(dpy,w);	break;
						case 38 :s=s+1;	XClearWindow(dpy,w);break;	
						case 39 :s=s-1;XClearWindow(dpy,w);break;
						case 42 :yc=yc+0.31415;	XClearWindow(dpy,w);break;
						case 43 :yc=yc-0.31415;XClearWindow(dpy,w);break;
						case 46 :nd=nd+1;	XClearWindow(dpy,w);break;	
						case 47 :nd=nd-1;XClearWindow(dpy,w);break;							
						case 44 :ny=ny+1;	XClearWindow(dpy,w);break;	
						case 45 :ny=ny-1;XClearWindow(dpy,w);break;
						case 40 :ku=ku+1;XClearWindow(dpy,w);	break;
						case 41 :ku=ku-1;XClearWindow(dpy,w);	break;	}	
					break;
	   case ButtonPress:
		 if(e.xany.window==quit)return 0;
			i=0;
			ku=1;
			s=1;
			XClearWindow(dpy,w);
		}
	}			
		else{
			XDrawString(dpy, quit, gc, 4, 10,"Exit", 5);
	        
			XDrawLine(dpy, w, gc,  170/*-50*cos(s)*/, 0, 170, 700);
	        XDrawLine(dpy, w, gc,  0/*-50*cos(s)*/, 170,  700, 170);
			si=si*9.8;
			
			 if(flug==0){XDrawPoint(dpy, w, gc,  170+i/10*cos(i/100.0), 170+i/10*sin(i/100.0));usleep(500);}
			else if(flug==1){XDrawPoint(dpy, w, gc,  170+100*sin(s*i/100.0), 170+100*sin((ku*i/100.0)+yc));}
			else if(flug==2){XDrawPoint(dpy, w, gc,  170+100*sin(i/100.0)*sin(i/100.0)*sin(i/100.0), 170+100*cos(i/100.0)*cos(i/100.0)*cos(i/100.0));usleep(500);}
			else if(flug==3){XDrawPoint(dpy, w, gc,  170+100*(cos(ny*i/100.0/nd))*(cos(i/100.0)), 170+100*(cos(ny*i/100.0/nd))*(sin(i/100.0)));usleep(500);}
			

			if(yc<=360||yc>=-360);

			if(i++>3000&&flug==4){i=0;
				
				
				}
			
			XFlush(dpy);
	     }
	}
      }













