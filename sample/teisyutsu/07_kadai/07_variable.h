#pragma once
#include <X11/Xlib.h>
#include <X11/Xutil.h>

Display *dpy;
Window w, quit,freq1,freq2,up_freq1,up_freq2,down_freq1,down_freq2,minus1,minus2;
Window up_size,sizer,down_size;
Window	amp1,amp2,up_amp1,up_amp2,down_amp1,down_amp2;
Window stoper,reset,back,next;
Window root;
int screen,ci;
int i,j,h,k,v,m, x, y,a;
int f1,f2;
int size=10,wave1_f=100,wave2_f=100,wave1_dig=5,wave2_dig=5;
int amp1_dig=3,amp2_dig=3;
int wave1_fu=1,wave2_fu=1,ss=2,size_place=12;
int mn=0;
int wave1_amp=100,wave2_amp=100;
int amp_sam,w1amp,w2amp;//,wwsize;
int base_f=128;
int stop=1;
int plus=2;
int change=1;
float num;
float ww;
char color_name[5][20]={"Cyan","MediumSpringGreen","dimgray","LightGrey","violet"};
char wave1_Hz[20],wave2_Hz[20],sizes[20],ws1[20],ws2[20],s_word[20]="STOP";
unsigned long black, white,color[8];
GC gc;
XEvent e;
Colormap cmap;
XColor c0,c1;
