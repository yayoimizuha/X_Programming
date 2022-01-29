#include <stdio.h>
#include <string.h>
#include <math.h>	/*　数学関数を使う　*/
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>

#define BORDER 2
#define WIDTH 1280
#define HIGHT 1224
int  main(int argc, char **argv){
	Display *dpy;
	Window w, quit;
 	Window root;
 	int screen;
 	unsigned long black, white;
 	GC gc;
 	XEvent e;
  int i=0,x,y,n,f,a;
 float p;
  char hello[]="Hello!";
  char question[]="sin=1,cos=2,tan=3";
  dpy = XOpenDisplay("");

  root = DefaultRootWindow(dpy);
  screen = DefaultScreen(dpy);
  white = WhitePixel(dpy, screen);
  black = BlackPixel(dpy, screen);

  w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
/*　サブウインドウを作る　*/
quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
gc = XCreateGC(dpy, w, 0, NULL);

XSelectInput(dpy, w, ButtonPressMask);
XSelectInput(dpy, quit, ButtonPressMask);	/*　サブウインドウ用　*/

XMapWindow(dpy, w);
XMapSubwindows(dpy, w);	/*　サブウインドウも表示　*/

XSetForeground(dpy, gc, black);
printf("which sim,cos,tan?(sin=1,cos=2,tan=3)\n");
scanf("%d",&n);
printf("phase(0~1)\n");
scanf("%f",&p);
printf("frequency?(loop count 1~9)\n");
scanf("%d",&f);
printf("amplitude?\n");
scanf("%d",&a);
while(1){

  if(XEventsQueued(dpy, QueuedAfterReading)){ /* for ground */
    XNextEvent(dpy, &e);
    switch(e.type){
      case ButtonPress :
        if(e.xany.window == quit)return 0;
        i=0;
        XClearWindow(dpy, w);
      }
    }else{

      	XDrawString(dpy, quit, gc, 4, 10, "Exit", 4);
      if(n==1){
        XDrawLine(dpy,w,gc,100,500,900,500); //x軸
        XDrawLine(dpy,w,gc,100,100,100,900); //y軸

          XDrawPoint(dpy,w,gc,100+i,a*(500-(290/f)*sin(i/(290.0/f)-p)));
      }
      if(n==2){
        XDrawLine(dpy,w,gc,100,500,900,500); //x軸
        XDrawLine(dpy,w,gc,100,100,100,900); //y軸
          XDrawPoint(dpy,w,gc,100+i,a*(500-(290/f)*cos(i/(290.0/f)-p)));

      }
      if(n==3){
        XDrawLine(dpy,w,gc,100,500,900,500); //x軸
        XDrawLine(dpy,w,gc,100,100,100,900); //y軸
        XDrawPoint(dpy,w,gc,100+i,a*(500-(290/f)*tan(i/(290.0/f)-p)));
      }
      i+=1;
    if(i>800) i=0;
    usleep(300);
    XFlush(dpy);
    }

}
}
