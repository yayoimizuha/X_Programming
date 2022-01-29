/* xdraw.c */
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <unistd.h>

#define BORDER 2
#define WIDTH  800	
#define HIGHT 800
#define SIZE 30
#define BOUND -0.8

int main(int argc,char **argv)
{
	Display *dpy;
	Window w;
	Window root;
	int    screen,painting,ball=255,countball=0;
	float px,py,i[ball],n[ball],ix[ball],vx[ball],vy[ball],nx,ny,vex[ball],vey[ball];
	int XLookupString(XKeyEvent *event, char *string , int len, KeySym *keysym_r, XComposeStatus *status);
	unsigned long black, white;
	GC       gc;
        XEvent e;
        char here[] = "Left Click:Balls Throw";
        char here2[] = "Right Click:Balls reset";
	
	dpy = XOpenDisplay("");
	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);
        XSelectInput(dpy,w,ButtonPressMask | ButtonReleaseMask | ButtonMotionMask | KeyPressMask );
	XMapWindow(dpy, w);
	

	
	while(1){
		reset:
		i[0]=0;
               	ix[0]=0;
		painting=0;
		countball=0;
		add:
		px=e.xbutton.x;
		py=e.xbutton.y;
		

	paintback:
	XNextEvent(dpy,&e);
	if(e.xbutton.button==3)goto reset;

	   
	   switch (e.type){
              case ButtonPress :
               if(e.xbutton.button==1){
               
                   XDrawString(dpy, w, gc ,10, 10,here, strlen(here));
                   XDrawString(dpy, w, gc ,10, 20,here2, strlen(here2));
                   
                   XDrawArc(dpy, w, gc,  e.xbutton.x-SIZE/2, e.xbutton.y-SIZE/2, SIZE, SIZE, 0, 360*64);
                   painting=1;
                   px=e.xbutton.x,py=e.xbutton.y;
                   n[countball]=0;
                     

               }
               break;
               
               case MotionNotify :
	        if(painting=1){
	         
			XDrawString(dpy, w, gc ,10, 10,here, strlen(here));
                   XDrawString(dpy, w, gc ,10, 20,here2, strlen(here2));
	         
	         XDrawArc(dpy, w, gc,  e.xbutton.x-SIZE/2, e.xbutton.y-SIZE/2, SIZE, SIZE, 0, 360*64);
	         XDrawLine(dpy, w, gc, e.xbutton.x, e.xbutton.y , px, py);

	         vx[countball]=e.xbutton.x-px;
	         vy[countball]=e.xbutton.y-py;
	         px=e.xbutton.x;
	         py=e.xbutton.y;
	         
	         goto paint;
	        }
	        break;
               
              case ButtonRelease :
                
              	
             	   vex[countball]=e.xbutton.x;
                   vey[countball]=e.xbutton.y;
               	   i[countball]=vy[countball];
               	   ix[countball]=vx[countball];
               	   painting=0;
               	   countball++;
               	   
              while(1){
              
              XDrawString(dpy, w, gc ,10, 10,here, strlen(here));
              XDrawString(dpy, w, gc ,10, 20,here2, strlen(here2));
      
      		if(XEventsQueued(dpy,QueuedAfterReading)){
      		  XNextEvent(dpy,&e);         	  
		   if(e.xbutton.button==3) goto reset;
		   if(e.xbutton.button==1) goto add;
		 }
		 
		 paint:

		 for(ball=0;ball<countball;ball++){
               		XDrawArc(dpy, w, gc,  vex[ball]-SIZE/2, vey[ball]+n[ball]-SIZE/2, SIZE, SIZE, 0, 360*64);
              		
               	 	
               	 	n[ball]+=i[ball];
               	 	i[ball]++;
               	 	vex[ball]+=ix[ball];
               	 	     	   
               	   if(vey[ball]+n[ball]>HIGHT||SIZE/2>vey[ball]+n[ball]||vex[ball]-SIZE/2>WIDTH||SIZE/2>vex[ball]-SIZE/2){
               	    if(vey[ball]+n[ball]>HIGHT){
               	    	XDrawArc(dpy, w, gc,  vex[ball]-SIZE/2, HIGHT-SIZE/2, SIZE, SIZE, 0, 360*64);
               	    	vey[ball]=HIGHT-SIZE/2;;
               	    	i[ball]=i[ball]*BOUND;
               	    	n[ball]=i[ball];
               	    	}//下端
               	    if(0>vey[ball]+n[ball]){
               	    	XDrawArc(dpy, w, gc,  vex[ball]-SIZE/2, SIZE/2, SIZE, SIZE, 0, 360*64);
               	    	vey[ball]=SIZE/2;;
               	    	i[ball]=i[ball]*BOUND;
               	    	n[ball]=i[ball];
               	    }//上端
               	    if(vex[ball]-SIZE/2>WIDTH) {
               	    	XDrawArc(dpy, w, gc,  WIDTH-SIZE/2, vey[ball]+n[ball]-SIZE/2, SIZE, SIZE, 0, 360*64);
               	    	vex[ball]=WIDTH-SIZE/2;;
               	    	ix[ball]=ix[ball]*BOUND;
               	    	
               	    }//右端
               	    if(0>vex[ball]-SIZE/2){ 
               	    	XDrawArc(dpy, w, gc, SIZE/2, vey[ball]+n[ball]-SIZE/2, SIZE, SIZE, 0, 360*64);
               	    	vex[ball]=SIZE/2;
               	    	ix[ball]=ix[ball]*BOUND;
               	    }//左端
               	   }
               	   


               	 }
               	   usleep(20000);
               	   XFlush(dpy);
               	   XClearWindow(dpy,w);	
               	   if(painting==1) goto paintback;
               	 }
               	 
               	 
               	 break;               	 	
               	 

              }

	      	
		 
	   }
}

