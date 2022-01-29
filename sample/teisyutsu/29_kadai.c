#include <stdio.h>
#include <math.h>    /* use math lib*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>
#include <unistd.h>

#define BORDER 2
#define WIDTH  1200
#define HIGHT 1000

int main(int argc, char **argv)
{
	
	float a,b,c,d;
	float p,f,g,h,z,Z;
	printf("三次関数の係数を表示してください。\n");
	printf("a=");
	scanf("%f",&a);//ax^3+bx^2+cx+d
	printf("b=");
	scanf("%f",&b);
	printf("c=");
	scanf("%f",&c);
	printf("d=");
	scanf("%f",&d);
	
	a=3.0*a;//微分したときの係数の変化
	b=2.0*b;//3ax^2+2bx+c
	p=b*b-(4.0*a*c);//解の公式のルートの部分の中身
	f=sqrtf(p);//ルート計算
	g=(-b+f)/(2*a);	//極値のX座標
	h=(-b-f)/(2*a);//極値のX座標
	z=(a*h*h*h)+(b*h*h)+(c*h)+d;//極値のY座標
	Z=(a*g*g*g)+(b*g*g)+(c*g)+d;//極値のY座標
	if(p>0){
		printf("極値（%f,%f）,(%f,%f) \n",h,z,g,z);//極値の座標を表示
	}
	else{
		printf("極値は存在しません。 \n");//極値の座標を表示
	}
	a=a/3;//係数を微分したものからもとの三次関数へもどす。
	b=b/2;

	Display *dpy;
	Window w,quit;
	Window root;
	int    screen;
	unsigned long black, white,green;
	GC       gc;
	XEvent   e;
	int  i,x,y;
	
	Colormap cmap;
	XColor c0,c1;
	char color_name[8][20]={"red","green","blue","pink","yellow","violet","brown","gold"};
	dpy = XOpenDisplay("");

	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);
		cmap = DefaultColormap(dpy,0);  /* カラーマップを得る */
	for(i=0;i<8;i++)
	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	/* Make Sub Window */
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);

	XSelectInput(dpy, w, ButtonPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);

	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);

	XSetForeground(dpy,gc,black);
	int Y = 0;
	int i2 = -1000;
	while(1){
	/* event loop */
		if(XEventsQueued(dpy, QueuedAfterReading)){
			XNextEvent(dpy, &e);
	    	switch(e.type){
	    		case ButtonPress : 
	   				if(e.xany.window == quit) return 0;
	   			break;
	   		}
	  	}
	  	else{
	    	XSetForeground(dpy,gc,black);
	    	XDrawString(dpy,quit,gc,4,10,"Exit",4);
	    	XDrawLine(dpy, w, gc, (WIDTH/2),0,(WIDTH/2),1000);//縦軸
        	i2++;
    		XDrawPoint(dpy, w, gc,i2+(WIDTH/2),(HIGHT/2));//横軸
			Y=(((a*i2*i2*i2)+(b*i2*i2)+(c*i2))/500000.0)+d;
			XAllocNamedColor(dpy,cmap,"blue",&c0,&c1);
			XSetForeground(dpy,gc,c1.pixel);
      		XDrawPoint(dpy, w, gc,i2+(WIDTH/2),Y+(HIGHT/2));
      		usleep(1);
      		XFlush(dpy);
		}
	      
	}

}

		
