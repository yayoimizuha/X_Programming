
          
                       
                 

       /* jisaku.c */
#include <stdio.h>
#include <math.h>	
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define BORDER 2
#define WIDTH 500
#define HIGHT 350

int  main(int argc, char **argv){
	Display *dpy;
	Window w, quit;
 	Window root;
 	int screen;
 	unsigned long black, white;
 	GC gc;
 	XEvent e;
	int i, X, Y, t, x, y, m, n;
       
      y=600;
   
	dpy = XOpenDisplay("");

	root = DefaultRootWindow(dpy);
	screen = DefaultScreen(dpy);
	white = WhitePixel(dpy, screen);
	black = BlackPixel(dpy, screen);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);

	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);

	XSelectInput(dpy, w, ButtonPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);	
	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);	

	XSetForeground(dpy, gc, black);
	while(1){
		if(XEventsQueued(dpy, QueuedAfterReading)){ /* for ground */
			XNextEvent(dpy, &e);
			switch(e.type){
				case ButtonPress :
					if(e.xany.window == quit)return 0;
					
                                       y=e.xbutton.y;
                                       
			}
		}else{	/* back ground */
			XDrawString(dpy, quit, gc, 4, 10, "Exit", 4);
                XDrawRectangle(dpy, w, gc, 50,50,350,600);
		
            XDrawArc(dpy, w, gc,  200, Y, 50, 50, 0, 360*64);  /* ball */
            XDrawLine(dpy,w,gc, 50, y, 400, y);
              if(Y<y){
                 
              if(0<i && i<y-100) Y=50+i;
                
              if(i>=y-100)  Y=2*(y-100)+50-i;
        
              if(i>=2*(y-100)) i=0;
              }
              if(Y>y){
                
              if((y-50)<i && i<550){
               
                Y=50+i;
                }
              if(i>=550 && 1100-i>=y-50)  Y=1100-i;
               
              if(i>=550 && 1100-i<=y-50)  Y=2*(y-50)-(1100-i);
 
              if(i>=1650) i=550;
              }
              
             
              i++;
              t=10000;
            usleep(t);
            XFlush(dpy);
           XClearWindow(dpy,w);
          
                       
                 

              }

                
        }
}

                       
                 
