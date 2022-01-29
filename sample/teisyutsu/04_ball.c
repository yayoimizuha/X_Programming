/* xdraw.c */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define BORDER 2
#define WIDTH  3500
#define HIGHT 2500
#define PI 3.141592

int main(int argc,char **argv){
	Display *dpy;
	Window w,quit;
	Window root;
	int screen,i;
	int dot[256][4];
	unsigned long black, white;
	GC gc; XEvent e;

	dpy = XOpenDisplay("");
	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);
	
	Colormap cmap; /* カラーマップ構造体 */
	XColor c0,c1; 
	char color_name[8][20]={"red","green","blue"};
	unsigned long c_no[8];
	
	float han,vzero,angle;
	int dotx,doty;
	printf("input coefficient of restitution(0<e<1)\n");
	scanf("%f",&han);
	printf("input initial speed(v0)\n");
	scanf("%f",&vzero);
	printf("input angle(0<θ<90)\n");
	scanf("%f",&angle);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);
	XSelectInput(dpy,w,ButtonPressMask | ButtonReleaseMask | KeyPressMask);
	XMapWindow(dpy, w);
	XSetForeground(dpy,gc,black);
	cmap = DefaultColormap(dpy,0);
	for(i=0;i<3;i++){
		XAllocNamedColor(dpy,cmap,color_name[i],&c1,&c0);  /* c1.pixel */
		c_no[i]=c1.pixel;
	}
	
	
	while(1){
		float Bx=0,By=600,vx,vy,g=9.8;
		int t=0,tt=0;
		XNextEvent(dpy,&e);
		switch (e.type){
			case ButtonPress :
				vx=vzero*sin(angle/180*PI);
				vy=vzero*cos(angle/180*PI);
				while(Bx>-100){
					if(By>830){
						vy=-fabs(han*vy);
						By=830;
					}
					if(By<0){
						vy=fabs(han*vy);
						By=0;
					}
					if(Bx>1100)vx=-fabs(han*vx);
						Bx+=vx/100;
						vy+=g/10;
						By+=vy/10;
						XClearWindow(dpy,w);
						XFillArc(dpy,w,gc,Bx-35,By-35,70,70,0,360*64);	
						XDrawLine(dpy,w,gc,0,865,1135,865);
						XDrawLine(dpy,w,gc,1135,0,1135,865);
/*						t++;
						if(t==256)tt++;
						dot[t-(tt*256)][(tt*2)]=Bx;
						dot[t-(tt*256)][(tt*2)+1]=By;
						for(i=0;i<t;i++){
							dotx=i;
							doty=0;
							if(i>=256){
								dotx-=256;
								doty++;
							}
							XDrawPoint(dpy,w,gc,dot[dotx][doty*2],dot[dotx][doty*2+1]);
						}*/
						XFlush(dpy);
						usleep(5000);
					}
				break;
		}
	}
}
