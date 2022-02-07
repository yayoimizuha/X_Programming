/*xdraw_ascii.c*/
#include<stdio.h>
#include<string.h>
#include<X11/Xlib.h>
#include<X11/Xutil.h>
#include<X11/keysym.h>

#define BORDER 2
#define WIDTH 350
#define HIGHT 250

int main(int argc,char **argv){
  Display *dpy;
  Window w;
  Window root;
  int screen;
  unsigned long black, white;
  GC gc;
  char here[9];
  XEvent e;
  KeySym key;
  char string[9];
  int l=0;
  
  dpy = XOpenDisplay("");
  root = DefaultRootWindow(dpy);
  screen = DefaultScreen(dpy);
  white = WhitePixel(dpy,screen);
  black = BlackPixel(dpy,screen);
  
  w = XCreateSimpleWindow(dpy,root,100,100,WIDTH,HIGHT,BORDER,black,white);
  gc = XCreateGC(dpy,w,0,NULL);
  
  XSelectInput(dpy,w,ButtonPressMask | ButtonReleaseMask | KeyPressMask);
  
  XMapWindow(dpy,w);
  
  while(1){
    XNextEvent(dpy,&e);
    switch(e.type){
    case ButtonPress:
      printf("x=%d, y=%d, button=%d\n", e.xbutton.x, e.xbutton.y, e.xbutton.button);
      if(e.xbutton.button==3)return(0);
      break;
      case ButtonRelease:
      XDrawPoint(dpy,w,gc,e.xbutton.x, e.xbutton.y);
      break;
      case KeyPress:
      if(XLookupString(&e, string, 9, &key, NULL)==1){
	here[0]=string[0];
	here[1]='\0';
	XDrawString(dpy,w,gc,30+l,200,here,strlen(here));
	l+=10;
      }
    }
  }
}
