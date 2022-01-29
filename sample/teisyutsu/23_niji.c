/* xdraw_sin.c */
#include <stdio.h>
#include <math.h>    /* use math lib*/
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>

#define BORDER 2
#define WIDTH  1200
#define HIGHT 900
void zahyou(Display *dpy, Window w, GC gc, float x, float y, float a, float b, float c);
float niji(float a, float b, float c, int i);

int main(int argc, char **argv)
{
	Display *dpy;
	Window w,quit;
	Window root;
	int    screen;
	unsigned long black, white;
	GC       gc;
	XEvent   e;
	int i, flag=0;
	float a,b,c,x1,x2,x3,y1,y2,y3;
	
	printf("１つめの点の座標を教えてください");
	scanf("%f %f",&x1,&y1);
	printf("2つめの点の座標を教えてください");
	scanf("%f %f",&x2,&y2);
	printf("3つめの点の座標を教えてください");
	scanf("%f %f",&x3,&y3);
	a=((y1-y2)*(x1-x3)-(y1-y3)*(x1-x2))/((x1-x2)*(x1-x3)*(x2-x3));
    b=((y1-y2)/(x1-x2)-a*(x1+x2));
    c=(y1-a*x1*x1-b*x1);
    printf("a=%f\nb=%f\nc=%f\n",a,b,c);

	Colormap cmap;             /* カラーマップ構造体 */
	XColor c0,c1;              /* カラー構造体 */
	
	        char color_name[8][20]={"red","green","blue","pink","purple","yellow","black","white"};
        unsigned long c_no[8];
        
	dpy = XOpenDisplay("");

	cmap = DefaultColormap(dpy,0);  /* カラーマップを得る */
		for(i=0;i<8;i++){
	        XAllocNamedColor(dpy,cmap,color_name[i],&c1,&c0);  /* c1.pixel */
	        c_no[i]=c1.pixel;
	}
	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	/* Make Sub Window */
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);

	XSelectInput(dpy, w, ButtonPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);

	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);

	XSetForeground(dpy,gc,black);
	
	while(1){
	/* event loop */
	   XNextEvent(dpy, &e);
	    switch(e.type){
	   case ButtonPress:
	      printf("x=%d y=%d button=%d \n",e.xbutton.x,e.xbutton.y,e.xbutton.button);
	     if(e.xany.window==quit)return 0;
	        XDrawString(dpy, quit, gc, 4, 10,"Exit", 4);
	     }
	   XDrawLine(dpy, w, gc, 600,0,600,1000);
	   XDrawLine(dpy, w, gc, 0,400,8000,400);
	   char AA[256],AAA[256],BB[256],BBB[256],CC[256],CCC[256];
	   sprintf(AA,"%f",x1);  sprintf(AAA,"%f",y1);
	   sprintf(BB,"%f",x2);  sprintf(BBB,"%f",y2);
	   sprintf(CC,"%f",x3);  sprintf(CCC,"%f",y3);
	   XSetForeground (dpy,gc,c_no[0]);
	   XDrawString(dpy, w, gc, 800, 680,"first point", 11);
	   XDrawString(dpy, w, gc, 800, 700,"x=", 2);XDrawString(dpy, w, gc, 850, 700,"y=", 2);
	   XDrawString(dpy, w, gc, 815, 700,AA, 3);  XDrawString(dpy, w, gc, 865, 700,AAA, 3);
	   XSetForeground (dpy,gc,c_no[1]);
	   XDrawString(dpy, w, gc, 800, 720,"second point", 12);
	   XDrawString(dpy, w, gc, 800, 740,"x=", 2);XDrawString(dpy, w, gc, 850, 740,"y=", 2);
	   XDrawString(dpy, w, gc, 815, 740,BB, 3);  XDrawString(dpy, w, gc, 865, 740,BBB, 3);
	   XSetForeground (dpy,gc,c_no[2]);
	   XDrawString(dpy, w, gc, 800, 760,"third point", 11);
	   XDrawString(dpy, w, gc, 800, 780,"x=", 2);XDrawString(dpy, w, gc, 850, 780,"y=", 2);
	   XDrawString(dpy, w, gc, 815, 780,CC, 3);  XDrawString(dpy, w, gc, 865, 780,CCC, 3);
	   XSetForeground (dpy,gc,c_no[6]);
	   for(i=-400;i<400;i++){
	    XDrawPoint(dpy, w, gc, 600+i*2,(niji(a,b,c,i)));
	    XDrawLine(dpy, w, gc, 599+i*2,(niji(a,b,c,i-1)),600+i*2,(niji(a,b,c,i)));
			   }
	   XSetForeground (dpy,gc,c_no[0]);
	    zahyou(dpy, w, gc, x1,y1,a,b,c);
		XSetForeground (dpy,gc,c_no[1]);
	    zahyou(dpy, w, gc, x2,y2,a,b,c);
		XSetForeground (dpy,gc,c_no[2]);
	    zahyou(dpy, w, gc, x3,y3,a,b,c);
	   XSetForeground (dpy,gc,c_no[6]);
	}
}
void zahyou(Display *dpy, Window w, GC gc, float x, float y, float a, float b, float c){
       XDrawPoint(dpy, w, gc, 600+x*2,((-1*((a*x*x)+(b*x)+c)/2)+400));
	   XDrawPoint(dpy, w, gc, 601+x*2,((-1*((a*x*x)+(b*x)+c)/2)+400));
	   XDrawPoint(dpy, w, gc, 599+x*2,((-1*((a*x*x)+(b*x)+c)/2)+400));
	   XDrawPoint(dpy, w, gc, 600+x*2,((-1*((a*x*x)+(b*x)+c)/2)+401));
	   XDrawPoint(dpy, w, gc, 600+x*2,((-1*((a*x*x)+(b*x)+c)/2)+399));
}
float niji(float a, float b, float c, int i){
	   float An;
       An=((-1*((a*i*i)+(b*i)+c))/2+400);
	   return An;
}


