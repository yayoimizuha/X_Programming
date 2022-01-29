/* xdraw.c */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define BORDER 2
#define WIDTH  500
#define HIGHT 500

int main(int argc,char **argv)
{
	Display *dpy;
	Window w,quit;
	Window root;
	int    screen,i=4,j=1;
	int x1,x2,x3,x4,y1,y2,y3,y4;
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
	gc = XCreateGC(dpy, w, 0, NULL);
        XSelectInput(dpy,w,ButtonPressMask | ButtonReleaseMask);
        XSelectInput(dpy,quit,ButtonPressMask);
	XMapWindow(dpy, w);
	XMapSubwindows(dpy,w);
	XSetForeground(dpy,gc,black);
	while(1){
	   XNextEvent(dpy,&e);
	   switch (e.type){
              case ButtonPress :
               if(e.xany.window==quit) return 0;
	        XDrawString(dpy, quit, gc, 4, 10,"Exit", 4);
	        XDrawPoint(dpy,w,gc,e.xbutton.x,e.xbutton.y);
		 if(i>0){
		 printf("x=%d y=%d button=%d \n",e.xbutton.x,e.xbutton.y,e.xbutton.button);
		 XDrawPoint(dpy,w,gc,e.xbutton.x,e.xbutton.y);
		 	if(i>3){
		 	x1=e.xbutton.x;
		 	y1=e.xbutton.y;
		 	i--;
		 	}
			 else if(i>2){
			 x2=e.xbutton.x;
			 y2=e.xbutton.y;
			 i--;
			 }
			 else if(i>1){
			 x3=e.xbutton.x;
			 y3=e.xbutton.y;
			 i--;
			 }
			 else if(i>0){
			 x4=e.xbutton.x;
		 	 y4=e.xbutton.y;
		 	i--;
		 	j--;
		 	float m1 = (y1-y2)/(x1-x2);
		 	float m2 = (y1-y3)/(x1-x3);
		 	float m3 = (y3-y2)/(x3-x2);
		 	float m4 = (y1-y4)/(x1-x4);
		 	float m5 = (y4-y2)/(x4-x2);
		 	float m6 = (y4-y3)/(x4-x3);
		 	float a=sqrt((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2));
			float b=sqrt((x3-x2)*(x3-x2)+(y3-y2)*(y3-y2));
			float c=sqrt((x1-x3)*(x1-x3)+(y1-y3)*(y1-y3));
			float s1=sqrt(((a+b+c)/2)*(((a+b+c)/2)-a)*(((a+b+c)/2)-b)*(((a+b+c)/2)-c));
			float d=sqrt((x1-x4)*(x1-x4)+(y1-y4)*(y1-y4));
			float e=sqrt((x3-x4)*(x3-x4)+(y3-y4)*(y3-y4));
			float f=sqrt((x2-x4)*(x2-x4)+(y2-y4)*(y2-y4));
			float s2=sqrt(((a+e+d)/2)*(((a+e+d)/2)-a)*(((a+e+d)/2)-d)*(((a+e+d)/2)-e));
			float s3=sqrt(((f+e+b)/2)*(((f+e+b)/2)-f)*(((f+e+b)/2)-b)*(((f+e+b)/2)-e));
			float s4=sqrt(((f+e+d)/2)*(((f+e+d)/2)-f)*(((a+e+d)/2)-d)*(((a+e+d)/2)-e));
			if((y3-(m1*(x3-x1)+y1))*(y4-(m1*(x4-x1)+y1))<0){
				XDrawLine(dpy,w,gc,x3,y3,x1,y1);
				XDrawLine(dpy,w,gc,x3,y3,x2,y2);
				XDrawLine(dpy,w,gc,x1,y1,x4,y4);
				XDrawLine(dpy,w,gc,x2,y2,x4,y4);
			}
			else if((y2-(m2*(x2-x3)+y3))*(y4-(m2*(x4-x3)+y3))<0){
				XDrawLine(dpy,w,gc,x4,y4,x1,y1);
				XDrawLine(dpy,w,gc,x3,y3,x2,y2);
				XDrawLine(dpy,w,gc,x3,y3,x4,y4);
				XDrawLine(dpy,w,gc,x2,y2,x1,y1);
			}
			else if((y2-(m4*(x2-x4)+y4))*(y3-(m4*(x3-x4)+y4))<0){
				XDrawLine(dpy,w,gc,x4,y4,x2,y2);
				XDrawLine(dpy,w,gc,x1,y1,x2,y2);
				XDrawLine(dpy,w,gc,x3,y3,x4,y4);
				XDrawLine(dpy,w,gc,x3,y3,x1,y1);
				}
			float s5=(s1+s2+s3+s4)/2;
			printf("s=%f \n",s5);
			
		 }
		 }
		 
		 break;
	      case ButtonRelease :
		 break;
	   }
	}
}

