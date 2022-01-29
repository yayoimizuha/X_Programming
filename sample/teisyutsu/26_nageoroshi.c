/* xdraw.c */
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#define BORDER 2
#define WIDTH  500
#define HIGHT 800

int main(int argc,char **argv)
{
	Display *dpy;
	Window w,quit,clear;
	Window root;
	int    screen,xx,yy,i,j,v,r,power,hx,hy,y,t;
	unsigned long black, white;
	GC       gc;
	char here[9];
   XEvent e;
   KeySym key;
   char string[9];
   int vo[10];
   int l=0;

	r=0;
	i=0;
	v=0;
	y=0;
	t=0.5;
	dpy = XOpenDisplay("");
	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	clear = XCreateSimpleWindow(dpy, w, 60, 3, 36, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);
   XSelectInput(dpy,w,ButtonPressMask | ButtonReleaseMask | KeyPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);	
	XSelectInput(dpy, clear, ButtonPressMask);
	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);	
	
	while(1){
	   if(XEventsQueued(dpy, QueuedAfterReading)){
	   	XNextEvent(dpy,&e);
	   	switch (e.type){
      	case ButtonPress :
      	 	if(e.xany.window == quit)return 0;
      	 	else if(e.xany.window == clear)XClearWindow(dpy, w);
      	 	r=0;
					i=0;
					v=0;
					t=0;
					y=0;
		 			printf("x=%d y=%d button=%d\n",e.xbutton.x,e.xbutton.y,e.xbutton.button);
		 			if(e.xbutton.button==3 || e.xbutton.button==1){
	 					xx=e.xbutton.x;
	 					yy=e.xbutton.y;
	 					hx=xx;
	 					hy=yy;
	 					l=0;
		 			}
		 			break;
	    	case ButtonRelease :
		 			XDrawArc(dpy,w,gc,e.xbutton.x-15,e.xbutton.y-15,30,30,0,360*64);
		 			break;
				case KeyPress :
					if(XLookupString(&e, string, 9, &key, NULL)==1){
						here[0]=string[0];
				 		here[1]='\0';
				 		if((e.xkey.keycode != 36) && (e.xkey.keycode < 20)){
				 			XDrawString(dpy, w, gc, xx+l, yy, here, strlen(here));
		 					l+=10;
		 					printf("\ni = %d \n",i);
							printf("key = %d \n",e.xkey.keycode);
							printf("go \n");
		 				}
					  if(e.xkey.keycode == 36){
						  vo[i]='\0';
						  printf("y= %d\n",y);
						  while(y<HIGHT){
							  XDrawArc(dpy,w,gc,hx-15,hy+y,30,30,0,360*64);
							  y=v*t+9.8*t*t;
							  XFlush(dpy);
							  usleep(100000);
							  printf("sleep \n");
							  t=t+1;
							}
					 }else if(e.xkey.keycode == 19){
						  vo[i]=0;
						  v=v*10;
					 }else if(e.xkey.keycode > 19){
						  XDrawString(dpy, w, gc, hx-45, hy-20,"Unavailable Value",17);
						  XDrawString(dpy, w, gc, 105, 15,"<- Please Clear",15);
						  break;
					 }else{
		 				  vo[i]=e.xkey.keycode - 9;
		 				  v=v*10+vo[i];
					}
					printf("v = %d \n",v);
					printf("vo = %d \n",vo[i]);
		 			i+=1;
		 		}
	   	}
		}else{
		XDrawString(dpy, quit, gc, 4, 10, "Exit", 4);
		XDrawString(dpy, clear, gc, 4, 10, "Clear", 5);
		XDrawString(dpy, w, gc, 15, 30, "Please click and enter the initial velocity at any place", 56);
	}
	}
}

