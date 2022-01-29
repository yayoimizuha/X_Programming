/* xkeyinput.c */
#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>  /* keysym */

#define BORDER 1
#define WIDTH  350
#define HIGHT 250

main(int argc, char **argv)
{
	Display *dpy;
	Window w,quit;
	Window root;
	int    screen;
	unsigned long black, white,c_no[7];
    char   c_name[7][20]={"red","blue","green","yellow","","",""};
	GC       gc;
	XEvent   e;
    Colormap cmap;
    XColor c0,c1;

	KeySym   key;  /* key シンボルを受け取るための変数 */
	char     text[30],string[10];
	int      p,mode;
	int      i;
	float    a;

	dpy = XOpenDisplay("");

	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);
    cmap = DefaultColormap(dpy,0);     /* Color no. set  */
    for(i=0;i<3;i++){
       XAllocNamedColor(dpy,cmap,c_name[i],&c1,&c0);
       c_no[i]=c1.pixel;
    }

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
    quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);
	XSelectInput(dpy,w, KeyPressMask|ButtonPressMask|ExposureMask);
    XSelectInput(dpy, quit, ButtonPressMask);
	XMapWindow(dpy, w);
    XMapSubwindows(dpy, w);
	p=0;mode=1; /* mode 1 is key input */
	a=0.0;
	while(1){
	  XNextEvent(dpy,&e);
	  switch (e.type){
	  case Expose :  XDrawString(dpy,quit,gc,4,10,"Exit",4);
		XDrawLine(dpy, w, gc, 175,50,175,200);
        XDrawLine(dpy, w, gc, 50,125,300,125);
        XDrawString(dpy,w,gc,60,50,"y=ax",4);
        XDrawString(dpy,w,gc,30,200,"a=?",3);
		if(mode==0){
		  sscanf(text,"%f",&a);   /* text to a */
		  XSetForeground(dpy,gc,c_no[0]);
		  for(i=-125;i<125;i++){
			XDrawPoint(dpy, w, gc, 175+i,125-i*a);
		  }
		  XSetForeground(dpy,gc,black);
		}
		break;
	  case ButtonPress : if(e.xany.window == quit)return ;
		if(e.xbutton.button==1){
		  mode=1;p=0;
		  XSetBackground(dpy,gc,white);
          XDrawImageString(dpy,w,gc,50,200,"        ",8);/* input aria CLR */
		}else mode=0;
		break;
	  case KeyPress:
		if(mode==1){
		  XLookupString(&e,string,10,&key,NULL);/* 警告出るが動く*/
		switch (key){
		case XK_Return: mode=0;break;
		case XK_BackSpace: --p;if(p<0)p=0;break;
		case XK_Shift_R:
		case XK_Shift_L: break;
		default: text[p++]=string[0];text[p]='\0';
		  XDrawString(dpy,w,gc,50,200,text,strlen(text));
		}
		if(mode==0){
		  sscanf(text,"%f",&a);   /* text to a */
		  XSetForeground(dpy,gc,c_no[0]);
		  for(i=-125;i<125;i++){
			XDrawPoint(dpy, w, gc, 175+i,125-i*a);
		  }
		  XSetForeground(dpy,gc,black);
		}
		}
	  }
	/* event loop */
	}
}

