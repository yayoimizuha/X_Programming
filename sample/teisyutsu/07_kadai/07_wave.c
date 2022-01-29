/* xsin_m.c */
#include <stdio.h>
#include <math.h>	
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "07_variable.h"

#define BORDER 2
#define WIDTH 1000
#define HIGHT 700
float ratio(int base,int frequency1,int frequency2);
int wave_point(int x,int time,float f,int swing);
int digit(int number);
void frequent(int *wave_f,int size,int *wave_dig,Window up_wave,Window down_wave,XEvent e);
void amplitude(int *wave_amp,int size,int *amp_dig,Window up_amp,Window down_amp,XEvent e);
void freq_adjus(int *bigger_freq,int *smaller_freq,int big_f,int small_f);
void mark(int *wave_mark,int *wave_freq);
void show();
void wave();
int  main(int argc, char **argv){
	dpy = XOpenDisplay("");
	root = DefaultRootWindow(dpy);
	screen = DefaultScreen(dpy);
	white = WhitePixel(dpy, screen);
	black = BlackPixel(dpy, screen);
	cmap = DefaultColormap(dpy,0);
	for(ci=0;ci<5;ci++){
		XAllocNamedColor(dpy,cmap,color_name[ci],&c1,&c0); 
		color[ci]=c1.pixel;
	}
	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, color[2]);
	/*　サブウインドウを作る　*/
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 60, 24, BORDER, black, white);
	freq1 = XCreateSimpleWindow(dpy, w, 800, 3, 60, 24, BORDER, black, white);
	freq2 = XCreateSimpleWindow(dpy, w, 800, 40, 60, 24, BORDER, black, white);
	up_freq1 = XCreateSimpleWindow(dpy, w, 860, 3, 30, 12, BORDER, black, white);
	down_freq1 = XCreateSimpleWindow(dpy, w, 860, 15, 30, 12, BORDER, black, white);
	up_freq2 = XCreateSimpleWindow(dpy, w, 860, 40, 30, 12, BORDER, black, white);
	down_freq2 = XCreateSimpleWindow(dpy, w, 860, 52, 30, 12, BORDER, black, white);
	minus1= XCreateSimpleWindow(dpy, w, 900, 3, 35, 24, BORDER, black, white);
	minus2= XCreateSimpleWindow(dpy, w, 900, 40, 35, 24, BORDER, black, white);
	up_size= XCreateSimpleWindow(dpy, w, 940, 3, 35, 20, BORDER, black, white);
	sizer= XCreateSimpleWindow(dpy, w, 940, 23, 35, 21, BORDER, black, white);
	down_size= XCreateSimpleWindow(dpy, w, 940, 44, 35, 20, BORDER, black, white);
	amp1 = XCreateSimpleWindow(dpy, w, 700, 3, 60, 24, BORDER, black, white);
	amp2 = XCreateSimpleWindow(dpy, w, 700, 40, 60, 24, BORDER, black, white);
	up_amp1 = XCreateSimpleWindow(dpy, w, 760, 3, 30, 12, BORDER, black, white);
	down_amp1 = XCreateSimpleWindow(dpy, w, 760, 15, 30, 12, BORDER, black, white);
	up_amp2 = XCreateSimpleWindow(dpy, w, 760, 40, 30, 12, BORDER, black, white);
	down_amp2 = XCreateSimpleWindow(dpy, w, 760, 52, 30, 12, BORDER, black, white);
	reset= XCreateSimpleWindow(dpy, w, 700, 77, 60, 24, BORDER, black, white);
	back= XCreateSimpleWindow(dpy, w, 800, 77, 20, 24, BORDER, black, white);
	stoper= XCreateSimpleWindow(dpy, w, 830, 77, 60, 24, BORDER, black, white);
	next= XCreateSimpleWindow(dpy, w, 900, 77, 20, 24, BORDER, black, white);

	gc = XCreateGC(dpy, w, 0, NULL);

	XSelectInput(dpy, w, ButtonPressMask);
	/*　サブウインドウ用　*/
	XSelectInput(dpy, quit, ButtonPressMask);	
	XSelectInput(dpy, freq1, ButtonPressMask);	
	XSelectInput(dpy, freq2, ButtonPressMask);	
	XSelectInput(dpy, up_freq1, ButtonPressMask);	
	XSelectInput(dpy, up_freq2, ButtonPressMask);	
	XSelectInput(dpy, down_freq1, ButtonPressMask);	
	XSelectInput(dpy, down_freq2, ButtonPressMask);	
	XSelectInput(dpy, minus1, ButtonPressMask);	
	XSelectInput(dpy, minus2, ButtonPressMask);	
	XSelectInput(dpy, up_size, ButtonPressMask);	
	XSelectInput(dpy, sizer, ButtonPressMask);	
	XSelectInput(dpy, down_size, ButtonPressMask);	
	XSelectInput(dpy, amp1, ButtonPressMask);
	XSelectInput(dpy, amp2, ButtonPressMask);
	XSelectInput(dpy, up_amp1, ButtonPressMask);
	XSelectInput(dpy, up_amp2, ButtonPressMask);
	XSelectInput(dpy, down_amp1, ButtonPressMask);
	XSelectInput(dpy, down_amp2, ButtonPressMask);
	XSelectInput(dpy, stoper, ButtonPressMask);
	XSelectInput(dpy, reset, ButtonPressMask);
	XSelectInput(dpy, back, ButtonPressMask);
	XSelectInput(dpy, next, ButtonPressMask);

	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);	/*　サブウインドウも表示　*/
	XSetForeground(dpy, gc, black);
	while(1){
		if(XEventsQueued(dpy, QueuedAfterReading)){ 
			XNextEvent(dpy, &e);
			switch(e.type){
				case ButtonPress :
					if(e.xany.window == quit)return 0;
					else{
						if(e.xany.window != w){
							change=1;
							if(stop==-1){
								XClearWindow(dpy, w);
								if(e.xany.window == back){
									if((j-size)>=0)	j-=size;
									else j=0;
								}
								else if(e.xany.window == next){
									if ((j+size)<=6431) j+=size;
									else j=0;
								}	
							}
							if((e.xany.window == up_freq1) || (e.xany.window == down_freq1) || (e.xany.window == minus1)){
								frequent(&wave1_f,size,&wave1_dig,up_freq1,down_freq1,e);
								XClearWindow(dpy, freq1);
							}
							else if((e.xany.window == up_freq2)||(e.xany.window == down_freq2) || (e.xany.window == minus2)){
								frequent(&wave2_f,size,&wave2_dig,up_freq2,down_freq2,e);
								XClearWindow(dpy, freq2);
							}
							else if((e.xany.window == up_amp1) || (e.xany.window == down_amp1)){
								amplitude(&wave1_amp,size,&amp1_dig,up_amp1,down_amp1,e);
								XClearWindow(dpy, amp1);
							}
							else if((e.xany.window == up_amp2)||(e.xany.window == down_amp2)){
								amplitude(&wave2_amp,size,&amp2_dig,up_amp2,down_amp2,e);
								XClearWindow(dpy, amp2);
							}
							else if(e.xany.window == up_size){
								size*=10;
								ss+=1;
								size_place-=3;
								if(size==10000){
									size_place=6;
									size=1000;
									ss=4;
								}
								XClearWindow(dpy, sizer);
							}
							else if(e.xany.window == down_size){
								if(size!=0){
									size/=10;
									ss-=1;
									size_place+=3;
								}
								if(size==0){
									size=1;
									ss=1;
									size_place=15;
								}
								XClearWindow(dpy, sizer);
							}
							else if(e.xany.window == stoper){
								stop=-stop;
								if(stop==1) sprintf(s_word,"STOP");
								else sprintf(s_word,"MOVE");
								XClearWindow(dpy, stoper);
							}
							else if(e.xany.window == reset){
								wave1_f=wave2_f=100;
								wave1_amp=wave2_amp=100;
								wave1_dig=wave2_dig=5;
								amp1_dig=amp2_dig=3;
								size=10;
								ss=2;
								size_place=12;
								XClearWindow(dpy,freq1);
								XClearWindow(dpy,freq2);
								XClearWindow(dpy,amp1);
								XClearWindow(dpy,amp2);
								XClearWindow(dpy,sizer);
							}
						}
					}
			}
		}
		else{	
			if(stop==1 || change==1){
				XClearWindow(dpy, w);
				show();
				if(wave1_f!=0 && wave2_f!=0){
					mark(&wave1_fu,&wave1_f);
					mark(&wave2_fu,&wave2_f);
					if(wave1_f>wave2_f)	freq_adjus(&f1,&f2,wave1_f,wave2_f);
					else if(wave1_f<wave2_f)	freq_adjus(&f2,&f1,wave2_f,wave1_f);
					else f1=f2=128;
					wave1_f=wave1_f*wave1_fu;
					wave2_f=wave2_f*wave2_fu;
					f1*=wave1_fu;
					f2*=wave2_fu;
					amp_sam=wave1_amp+wave2_amp;
					w1amp=ratio(200,wave1_amp,amp_sam);
					w2amp=ratio(200,wave2_amp,amp_sam);
					if(w1amp>w2amp){
						if(w2amp<=2){
							w1amp-=2;
							w2amp+=2;
						}
					}
					if(w2amp>w1amp){
						if(w1amp<=2){
							w2amp-=2;
							w1amp+=2;
						}
					}
				}
				else if(wave1_f == 0){
					f2=128;
					if(wave2_f < 0) f2=-f2;
					w2amp=100;
				}
				else if(wave2_f == 0){
					f1=128;
					if(wave1_f < 0) f1=-f1;
					w1amp=100;
				}
				if(stop==1)	plus=2;
				else plus=1;
				wave();
				change=0;
				usleep(15000);
				XFlush(dpy);
			}
		}
	}
}
float ratio(int base,int frequency1,int frequency2){
	float rat,ff1=frequency1,ff2=frequency2;
	rat=base*ff1/ff2;
	return rat;
}
int wave_point(int x,int time,float f,int swing){
	int point;
	point=swing*sin((x-time)/f);
	return point;
}
int digit(int number){
	int dig;
	while(number!=0){
		number = number / 10;
		++dig;
	}
	return dig;
}
void frequent(int *wave_f,int size,int *wave_dig,Window up_wave,Window down_wave,XEvent e){
	int freq,dig;
	freq=*wave_f;
	if(e.xany.window == up_wave){
		if((freq+size)<=9999)	freq+=size;
		else	freq=9999;
	}
	else if(e.xany.window == down_wave){
		if((freq-size)>-10000)	freq-=size;
		else	freq=-9999;
	}
	else	freq=-freq;
	dig=digit(freq)+2;
	if(freq < 0)	dig+=1;
	*wave_f=freq;
	*wave_dig=dig;
}
void amplitude(int *wave_amp,int size,int *amp_dig,Window up_amp,Window down_amp,XEvent e){
	int amp;
	amp=*wave_amp;
	if(e.xany.window == up_amp){
		if((amp+size)<10000)	amp+=size;
		else amp=9999;
	}
	else  if(e.xany.window == down_amp){
		if((amp-size)>0)	amp-=size;
		else amp=1;
	}
	*amp_dig=digit(amp);
	*wave_amp=amp;
}
void freq_adjus(int *bigger_freq,int *smaller_freq,int big_f,int small_f){
	float f=1;
	int base_f=128;
	while(f<10){
		f=ratio(base_f,small_f,big_f);
		if(f<10) base_f+=32;
	}
	*bigger_freq=f;
	*smaller_freq=base_f;
}
void mark(int *wave_mark,int *wave_freq){
	int freq=*wave_freq;
	if(freq<0){
		*wave_mark=-1;
		*wave_freq=-freq;
	}
	else *wave_mark=1;
}
void show(){
	XSetForeground(dpy, gc, black);
	XDrawString(dpy, quit, gc, 18, 14, "Exit", 4);
	XDrawString(dpy, up_freq1, gc, 10, 7, "up", 2);
	XDrawString(dpy, down_freq1, gc, 4, 9, "down", 4);
	XDrawString(dpy, up_freq2, gc, 10, 7, "up", 2);
	XDrawString(dpy, down_freq2, gc, 4, 9, "down", 4);
	XDrawString(dpy, minus1, gc, 4, 15, "minus", 5);
	XDrawString(dpy, minus2, gc, 4, 15, "minus", 5);
	XDrawString(dpy, up_amp1, gc, 10, 7, "up", 2);
	XDrawString(dpy, down_amp1, gc, 4, 9, "down", 4);
	XDrawString(dpy, up_amp2, gc, 10, 7, "up", 2);
	XDrawString(dpy, down_amp2, gc, 4, 9, "down", 4);
	XDrawString(dpy, stoper, gc, 17, 17,s_word , 4);
	XDrawString(dpy, reset, gc, 17, 17,"RESET",5);
	XDrawString(dpy, up_size, gc, 10, 12, "up", 2);
	XDrawString(dpy, down_size, gc, 6, 13, "down", 4);
	XDrawString(dpy, back, gc, 8, 16,"<", 1);
	XDrawString(dpy, next, gc, 8, 16,">",1);
	sprintf(wave1_Hz,"%dHz",wave1_f);
	sprintf(ws1,"%d",wave1_amp);
	sprintf(wave2_Hz,"%dHz",wave2_f);
	sprintf(ws2,"%d",wave2_amp);
	sprintf(sizes,"%d",size);
	XDrawString(dpy, sizer, gc, size_place, 15, sizes,ss);
	XSetForeground (dpy,gc,color[0]);
	XDrawString(dpy, freq1, gc, 10, 18, wave1_Hz, wave1_dig);
	XDrawString(dpy, amp1, gc, 10, 18, ws1, amp1_dig);
	XDrawString(dpy, w, gc, 665, 23,"wave1",5);
	XSetForeground (dpy,gc,color[1]);
	XDrawString(dpy, freq2, gc, 10, 18, wave2_Hz, wave2_dig);
	XDrawString(dpy, amp2, gc, 10, 18, ws2, amp2_dig);
	XDrawString(dpy, w, gc, 665, 60,"wave2",5);
	XSetForeground(dpy, gc,black);
	XDrawLine(dpy, w, gc, 100, 350, 901, 350);	/* x軸 */
	XDrawLine(dpy, w, gc, 100, 100, 100, 600);		/* y軸 */
	XSetForeground (dpy,gc,color[3]);
	XDrawLine(dpy, w, gc, 101,400, 901, 400);	
	XDrawLine(dpy, w, gc, 101, 450, 901, 450);	
	XDrawLine(dpy, w, gc, 101, 500, 901, 500);	
	XDrawLine(dpy, w, gc, 101, 550, 901, 550);	
	XDrawLine(dpy, w, gc, 101, 300, 901, 300);
	XDrawLine(dpy, w, gc, 101, 250, 901, 250);
	XDrawLine(dpy, w, gc, 101, 200, 901, 200);
	XDrawLine(dpy, w, gc, 101, 150, 901, 150);
}
void wave(){
	for(i=0;i<=800;i+=plus){
		if((stop==-1||(i%3)==0) && wave1_f!=0){
			XSetForeground (dpy,gc,color[0]);
			h=350-wave_point(i,j,f1,w1amp);
			XDrawPoint(dpy, w, gc, 101+i,h);
		}
		if((stop==-1||(((i%6)==0)|| (i%6)==1) )&&wave2_f!=0){
			XSetForeground (dpy,gc,color[1]);
			k=350-wave_point(i,j,f2,w2amp);
			XDrawPoint(dpy, w, gc, 101+i,k);
		}
		if(wave1_f!=0 && wave2_f!=0){
			XSetForeground (dpy,gc,color[4]);
			v=350-wave_point(i,j,f1,w1amp)-wave_point(i,j,f2,w2amp);
			XDrawPoint(dpy, w, gc, 101+i,v);
		}
	}
	if(stop==1){
		if(j++>6430)j=0;
	}
}
