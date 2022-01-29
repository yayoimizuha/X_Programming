/* xdraw_wave.c */
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>
#include <stdio.h>
#include <math.h>
#include <unistd.h>
#define BORDER	2
#define WIDTH	750
#define HIGHT	450
#define width 550
int main(int argc,char **argv){
	Display *dpy;
	Window w,quit,MC;
	Window root;
	int screen;
	unsigned long black, white;
	GC gc;
	Colormap cmap;
	XColor c0,c1;
	dpy = XOpenDisplay("");
	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	quit = XCreateSimpleWindow(dpy, w, 600, 5, 50, 20, BORDER, black, white);
	MC = XCreateSimpleWindow(dpy, w, 600, 75, 130, 20, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);

	int i,j,mode=0;
	int xRight[16]={},xLeft[16]={};
	float yRight[16]={},yLeft[16]={},wave[width]={};
	XEvent e;
	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);
	XSelectInput(dpy,w,ButtonPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);
	XMapSubwindows(dpy, w);
	XSelectInput(dpy, MC, ButtonPressMask);
	int button;
	XFlush(dpy);

	while(1){
		if(XEventsQueued(dpy,QueuedAfterReading)){
			XNextEvent(dpy,&e);
			if(e.xany.window == MC){
				if(mode==0)
				mode=1;
				else if(mode==1)
				mode=2;
				else if(mode==2)
				mode=0;
			}
			if(e.xany.window == quit)
				return 0;
			i=0;
			
			while((yRight[i]*yRight[i]>3)||(yLeft[i]*yLeft[i]>3)){
				i++;
				if(i>14){
					yRight[0]=0;
					yLeft[0]=0;
					i=0;
				}
			}
			if(e.xbutton.x<width){
				xRight[i]=e.xbutton.x;
				xLeft[i]=xRight[i];
				yRight[i]=(e.xbutton.y-HIGHT/2)*3/5;
				yLeft[i]=yRight[i];
			}
		}
		else{
			XClearWindow(dpy,w);
			for(j=0;j<width;j++)
				wave[j]=0;
			for(i=0;i<16;i++){
				xRight[i]++;
				xLeft[i]--;
				yRight[i]=0.0+yRight[i]*0.993;							//波の減衰度合い1が最大
				yLeft[i]=0.0+yLeft[i]*0.993;
				if(xRight[i]==width-15){							//右端設定 自由端
					j=0;
					while(yLeft[j]*yLeft[j]>3){
						j++;
						if(j>15){
							yLeft[0]=0;
							j=0;
						}
					}
					xLeft[j]=width+15;
					yLeft[j]=yRight[i];
				}
				if(xRight[i]>=width+15)
					xRight[i]=0,yRight[i]=0;
				if(mode==0){				//左端設定A	何もなし
						if(xLeft[i]<=-15)
						xLeft[i]=0,yLeft[i]=0;
					}
				else if(mode==1){						//左端設定B	固定端
					if(xLeft[i]==15){
						j=0;
						while(yRight[j]*yRight[j]>3){
							j++;
							if(j>15){
								yRight[0]=0;
								j=0;
							}
						}
						xRight[j]=-15;
						yRight[j]=-yLeft[i];
					}
					if(xLeft[i]<=-15)
					xLeft[i]=0,yLeft[i]=0;
				}
				
				else if(mode==2){						//左端設定C	自由端
					if(xLeft[i]==15){
						j=0;
						while(yRight[j]*yRight[j]>3){
							j++;
							if(j>15){
								yRight[0]=0;
								j=0;
							}
						}
						xRight[j]=-15;
						yRight[j]=yLeft[i];
					}
					if(xLeft[i]<=-15)
					xLeft[i]=0,yLeft[i]=0;
				}
				
				for(j=-15;j<15;j++){
					wave[xRight[i]+j]+=yRight[i]*cos(j/10.0);
					wave[xLeft[i]+j]+=yLeft[i]*cos(j/10.0);
				}
			}
			for(i=0;i<width-1;i++){
				XSetForeground(dpy,gc,(i+50)*100);
				XDrawLine(dpy,w,gc,i,wave[i]+HIGHT/2,i+1,wave[i+1]+HIGHT/2);
			}
				XSetForeground(dpy,gc,black);
				XDrawString(dpy, quit, gc, 15, 15, "Exit", 4);
				
			if(mode==0){
				XDrawString(dpy, w, gc, 600, 60, "Now mode0: Left free", 20);
				XSetForeground(dpy,gc,white);
				XDrawString(dpy, MC, gc, 15, 15, "mode change to 0!", 17);
				XSetForeground(dpy,gc,black);
				XDrawString(dpy, MC, gc, 15, 15, "mode change to 1!", 17);
			}
			if(mode==1){
				XDrawString(dpy, w, gc, 600, 60, "Now mode1: Left koteitan", 24);
				XSetForeground(dpy,gc,white);
				XDrawString(dpy, MC, gc, 15, 15, "mode change to 1!", 17);
				XSetForeground(dpy,gc,black);
				XDrawString(dpy, MC, gc, 15, 15, "mode change to 2!", 17);
			}
			if(mode==2){
				XDrawString(dpy, w, gc, 600, 60, "Now mode2: Left ziyuutan", 24);
				XSetForeground(dpy,gc,white);
				XDrawString(dpy, MC, gc, 15, 15, "mode change to 2!", 17);
				XSetForeground(dpy,gc,black);
				XDrawString(dpy, MC, gc, 15, 15, "mode change to 0!", 17);
			}

			XSetForeground(dpy,gc,black);
			XDrawRectangle(dpy,w,gc,0,0,width,HIGHT-1);
			XDrawRectangle(dpy,w,gc,550,0,200,HIGHT-1);
			
			XFlush(dpy);
			usleep(50000);
		}
	}
}
