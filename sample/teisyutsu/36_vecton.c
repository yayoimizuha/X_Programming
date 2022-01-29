/* xdraw_sub.c */
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <X11/Xlocale.h>

#define BORDER 2
#define WIDTH  600
#define HIGHT 450

typedef float scalar;
typedef struct{
	scalar x;
	scalar y;
}vector;

scalar norm(vector c);
vector wa(vector c1, vector c2);
vector sa(vector c1, vector c2);
float naiseki(vector c1, vector c2);
float kaku(vector c1, vector c2);

float norm(vector c){
	return sqrt(c.x*c.x+c.y*c.y);	
}

vector wa(vector c1,vector c2){
	vector d;
	d.x = c1.x+c2.x;
	d.y = c1.y+c2.y;
return d;
}

vector sa(vector c1,vector c2){
	vector d;
	d.x = c1.x-c2.x;
	d.y = c1.y-c2.y;
	return d;
}

float naiseki(vector c1,vector c2){
	return c1.x*c2.x+c1.y*c2.y;	
}

float kaku(vector c1,vector c2){
	return acos(naiseki(c1,c2) / (norm(c1)*norm(c2)));	
}

int main(int argc, char **argv)
{
	Display *dpy;
	Window w,w1,w2,w3,w4;
	Window root;
	int    i,screen,xx,yy,j;
	vector a,b;
	vector ans;
	float pi=3.141592;
	unsigned long black, white;
	GC gc;
	XEvent e;
	char moji[256],moja[256],moju[256],moje[256],mojo[256],monja[256],monmon[256],tyui[256];
	int Flag=0;


	XFontSet fs;
	char **miss,*def;
	int n_miss;

	setlocale(LC_ALL,"");

	dpy = XOpenDisplay("");

	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);

	w1 = XCreateSimpleWindow(dpy, w, 60, 3, 50, 12, BORDER, black, white);

	w2 = XCreateSimpleWindow(dpy, w, 10, 3, 40, 12, BORDER, black, white);

	w3 = XCreateSimpleWindow(dpy, w, 10, 50, 580, 290, BORDER, black, white);

	w4 = XCreateSimpleWindow(dpy, w, 10, 350, 580, 90, BORDER, black, white);

	gc = XCreateGC(dpy, w, 0, NULL);
	XSelectInput(dpy,w1,ButtonPressMask);
	XSelectInput(dpy,w2,ButtonPressMask);
	XSelectInput(dpy,w3,ButtonPressMask | ButtonReleaseMask);

	fs = XCreateFontSet(dpy,"-*-*-medium-r-normal-*-16*-*",&miss,&n_miss,&def);

	XMapWindow(dpy, w);
	XMapSubwindows(dpy,w);

        XSetForeground(dpy,gc,black);


	while(1){
		XNextEvent(dpy, &e);
		sprintf(moji,"Draw two lines");
		XmbDrawString(dpy,w,fs,gc,10,45,moji,strlen(moji));
		sprintf(tyui,"please match the start point");
		XmbDrawString(dpy,w3,fs,gc,3,10,tyui,strlen(tyui));
		switch (e.type){
			case ButtonPress :
				XDrawString(dpy,w2,gc,3,10,"Exit",4);
				XDrawString(dpy,w1,gc,4,10,"Reset",5);
				if (e.xany.window == w2){ 
					return 0;
				}
				if (e.xany.window == w1){
				XClearWindow(dpy,w3);
				XClearWindow(dpy,w4);
				Flag=0;
				}
				xx=e.xbutton.x;
				yy=e.xbutton.y;
				XDrawPoint(dpy, w3, gc, e.xbutton.x, e.xbutton.y);
				break;

			case ButtonRelease :
				XDrawLine(dpy, w3, gc,xx,yy, e.xbutton.x, e.xbutton.y);
				
				if(Flag==0){
				a.x=e.xbutton.x-xx;
				a.y=e.xbutton.y-yy;
				Flag=1;
				}
				else if(Flag==1){
				b.x=e.xbutton.x-xx;
				b.y=e.xbutton.y-yy;
				Flag=2;
				}
				if(a.x<0){
				a.x=-a.x;
				}
				if(a.y<0){
				a.y=-a.y;
				}
				if(b.x<0){
				b.x=-b.x;
				}
				if(b.y<0){
				b.y=-b.y;
				}
				break;
				}
		if(Flag==2){
		sprintf(moja,"a=(%3.2f,%3.2f), b=(%3.2f,%3.2f)",a.x,a.y,b.x,b.y);
		XmbDrawString(dpy,w4,fs,gc,3,20,moja,strlen(moja));
		sprintf(moje,"a norm=%3.2f, b norm=%3.2f",norm(a),norm(b));
		XmbDrawString(dpy,w4,fs,gc,3,40,moje,strlen(moje));
		ans=wa(a,b);
		sprintf(mojo,"wa=(%3.1f,%3.1f)",ans.x,ans.y);
		XmbDrawString(dpy,w4,fs,gc,3,60,mojo,strlen(mojo));
		ans=sa(a,b);
		sprintf(moju,"sa=(%3.1f,%3.1f)",ans.x,ans.y);
		XmbDrawString(dpy,w4,fs,gc,200,60,moju,strlen(moju));
		sprintf(monja,"naiseki=%5.3f",naiseki(a,b));
		XmbDrawString(dpy,w4,fs,gc,3,80,monja,strlen(monja));
		sprintf(monmon,"kaku=%3.2f[deg]",kaku(a,b)*180/pi);
		XmbDrawString(dpy,w4,fs,gc,200,80,monmon,strlen(monmon));
		}
	}
}
