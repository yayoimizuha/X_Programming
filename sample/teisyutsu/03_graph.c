#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <X11/Xlib.h>	
#include <X11/Xlocale.h>
#include <X11/keysym.h>
#include <stdlib.h>
#include <math.h>

#define BORDER 2
#define WIDTH 1000
#define HIGHT 800

float fn(int i,int a,int *keisu);
float taisu(int i,float a);
float sisu(int i,float a);
float logn(int base,float antilog);
float bibun(int i,int a,int *keisu);
int main(int argc,char **argv){
	Display *dpy;
	Window w,quit;
	Window root;
	XFontSet fs;
	GC gc;
	XEvent e;
	int a,b,c,i,j,k,flag,flagw,flagm,flagf,screen,n_miss;
	int *keisu = (int *)malloc(sizeof(int)*10);
	float teit,teis,y,yp,yzahyou,xzahyou;
	unsigned long black, white;
	char moji[256],**miss,*def;
	i = 0;
	flag = 0;
	flagw = 0;
	flagm = 0;
	flagf = 0;

	setlocale(LC_ALL,"");
	
	dpy = XOpenDisplay("");

	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy,screen);
	black = BlackPixel (dpy,screen);
	
	w = XCreateSimpleWindow(dpy,root,100,100,WIDTH,HIGHT,BORDER,black,white);
	quit = XCreateSimpleWindow(dpy,w,10,1,35,15,BORDER,black,white);

	gc = XCreateGC(dpy,w,0,NULL);
	XSetBackground(dpy,gc,white);

	fs = XCreateFontSet(dpy,"-*-*-medium-r-normal-*-16-*",&miss,&n_miss,&def);
	
	XSelectInput(dpy,w,ButtonPressMask | KeyPressMask);
	XSelectInput(dpy,quit,ButtonPressMask);

	XMapWindow(dpy,w);
	XMapSubwindows(dpy,w);

	while(1){
		if(XEventsQueued(dpy,QueuedAfterReading)){
			XNextEvent(dpy, &e);
			switch(e.type){
				case ButtonPress :
					if(e.xany.window == quit) return 0;
			    	XClearWindow(dpy,w);
				case KeyPress :
					a = e.xkey.keycode;
                    if(a == 20) flagm = 1;
					if(a == 46) flag = 1;
					if(a == 39) flag = 2;
					if(9 < a && a < 20 || a == 26){
						switch(flag){
							case 0 :
								if(a == 19) b = 19;
								else b = 9;
								switch(flagm){
									case 0 :
										keisu[i] = a-b;
                                    	sprintf(moji,"%d ",keisu[i]);
                                    	XmbDrawString(dpy,w,fs,gc,120+i*20,290,moji,strlen(moji));
                                    	i++;
										break;
									case 1 :
										keisu[i] = (a-b)*(-1);
										flagm = 0;
                                    	sprintf(moji,"%d ",keisu[i]);
                                    	XmbDrawString(dpy,w,fs,gc,120+i*20,290,moji,strlen(moji));
										i++;
										break;
								}
						    	break;
							case 1 :
								if(9 < a && a < 20){
									if(a == 19) b = 19;
									else b = 9;
									teit = a-b;
								}
								if(a == 26) teit = expf(1);
								flagw = 1;
								break;
							case 2 :
								if(9 < a && a < 20){
									if(a == 19) b = 19;
									else b = 9;
									teis = a-b;
								}
								if(a == 26) teis = expf(1);
								flagw = 1;
								break;
						}
					}
					if(a == 24 && flag == 0 && flagf == 0){
						sprintf(moji,"y =");
						XmbDrawString(dpy,w,fs,gc,120,310,moji,strlen(moji));
						for(k=0;k<i;k++){
							if(k < i-1){
								if(keisu[k] != 1){
									sprintf(moji," %dx^%d +",keisu[k],i-k-1);
									XmbDrawString(dpy,w,fs,gc,150+k*50,310,moji,strlen(moji));
								}
								if(keisu[k] == 1){
									sprintf(moji," x^%d +",i-k-1);
									XmbDrawString(dpy,w,fs,gc,150+k*50,310,moji,strlen(moji));
								}
							}
							if(k == i-1){
								sprintf(moji," %d",keisu[k]);
								XmbDrawString(dpy,w,fs,gc,150+k*50,310,moji,strlen(moji));
							}
						}
						flagf = 1;
						break;
					}
					if(a == 24 && flagf == 1) flagw = 1;
			}
		}
		if(flagw == 0){
			XSetForeground (dpy,gc,black);
			XDrawString(dpy,quit,gc,3,14,"Exit",4);
			sprintf(moji,"グラフに表したい関数の係数(一桁の整数、負も可)を次数の大きい順に入力してください、");
			XmbDrawString(dpy,w,fs,gc,120,250,moji,strlen(moji));
			sprintf(moji,"定数項まで入力したら'q'を入力して関数を確認してもう一度'q'を入力してください： ");
			XmbDrawString(dpy,w,fs,gc,120,270,moji,strlen(moji));
			sprintf(moji,"対数関数のグラフを表示させたいときは'l'を入力してから底（一桁の正の整数）を入力してください、");
			XmbDrawString(dpy,w,fs,gc,120,350,moji,strlen(moji));
			sprintf(moji,"自然対数のグラフは'l'のあとに'e'を入力してください：");
			XmbDrawString(dpy,w,fs,gc,120,370,moji,strlen(moji));
			sprintf(moji,"指数関数のグラフを表示させたいときは's'を入力してから底（一桁の正の整数）を入力ください、");
			XmbDrawString(dpy,w,fs,gc,120,410,moji,strlen(moji));
			sprintf(moji,"底をネイピア数にしたいときは's'のあとに'e'を入力してください： ");
			XmbDrawString(dpy,w,fs,gc,120,430,moji,strlen(moji));
		}
        if(flagw == 1){
            XClearWindow(dpy,w);
			XDrawString(dpy,quit,gc,3,14,"Exit",4);
            XDrawLine(dpy,w,gc,125,400,875,400); //x
			XDrawLine(dpy,w,gc,875,400,870,405);
			XDrawLine(dpy,w,gc,875,400,870,395);
			XDrawString(dpy,w,gc,879,404,"x",1);
			XDrawLine(dpy,w,gc,500,25,500,775); //y
			XDrawLine(dpy,w,gc,500,25,505,30);
			XDrawLine(dpy,w,gc,500,25,495,30);
			XDrawString(dpy,w,gc,497,15,"y",1);
			XDrawString(dpy,w,gc,485,390,"O",1); //o
            switch(flag){
                case 0 : 
                    for(j=-375;j<376;j++){
						y = fn(j,i-1,keisu);
						yp = bibun(j,i-1,keisu);
						XDrawPoint(dpy,w,gc,500+j,400-y);
						xzahyou = j/100.0;
						yzahyou = y/100.0;
						if(j == 0){
							sprintf(moji,"(0.00,%3.2f) ",yzahyou);
                            XmbDrawString(dpy,w,fs,gc,510,400-y,moji,strlen(moji));
						}
						if(y == 0){
							sprintf(moji,"(%3.2f,0.00) ",xzahyou);
                            XmbDrawString(dpy,w,fs,gc,500+j,420,moji,strlen(moji));
						}
						if(yp == 0){
							sprintf(moji,"(%3.2f,%3.2f)",xzahyou,yzahyou);
							XmbDrawString(dpy,w,fs,gc,500+j,420-y,moji,strlen(moji));
						}
					}
                    break;
                case 1 :
                    for(j=1;j<376;j++){
						y = taisu(j,teit);
						XDrawPoint(dpy,w,gc,500+j,400-y);
						if(j == 1){
							sprintf(moji,"(1,0) ");
                            XmbDrawString(dpy,w,fs,gc,600,420,moji,strlen(moji));
						}
					}
                    break;
                case 2 :
                    for(j=-375;j<376;j++){
						y = sisu(j,teis);
						XDrawPoint(dpy,w,gc,500+j,400-y);
						if(j == 0){
							sprintf(moji,"(0,1) ");
                            XmbDrawString(dpy,w,fs,gc,510,300,moji,strlen(moji));
						}
					}
                    break;
			}
            usleep(5000);
			XFlush(dpy);
        }
	}
    free(keisu);
}
float fn(int i,int a,int *keisu){
	int j;
	float x,y;
	x = i/100.0;
	y = 0;
	for(j=0;j<=a;j++) y += keisu[j]*pow(x,a-j);
	return 100*y;	
}
float taisu(int i,float a){
	int j;
	float x,y;
	x = i/100.0;
	y = logn(a,x);
	return 100*y;
}
float sisu(int i,float a){
	float x,y;
	x = i/100.0;
	y = pow(a,x);
	return 100*y;
}
float logn(int base,float antilog){
	return log(antilog) / log((float)base);
}
float bibun(int i,int a,int *keisu){
	int j,k;
	float x,yp;
	x = i/100.0;
	for(j=0;j<=a;j++){
		k = a-j;
		yp += keisu[j]*k*pow(x,k-1);
	}
	return yp;
}
