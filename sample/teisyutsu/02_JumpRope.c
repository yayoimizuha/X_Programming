#include <stdio.h>
#include <math.h>
#include <unistd.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>
#include <time.h>
int main(int argc,char **argv)
{
        #define BORDER 2
        #define WIDTH  800 
        #define HIGHT 600
        //↑Edit display size

        Display *dpy;
        int    screen;
        GC       gc;
	XEvent  e;
        //↑Configure display(DO NOT CHANGE)

        char moji[256];
        char Sscore[256];
        XFontSet fs[2];
        char **miss,*def;
        int n_miss;
        setlocale(LC_ALL,"");
        //↑Configure Japanese text(DO MOT CHANGE)
        
        Window base,exit;
        Window root;
        //↑Configure windows

        unsigned long black, white;
        char color_name[7][20]={"red","green","blue","yellow","magenta","cyan","gray"};
        Colormap cmap;
        XColor c0,c1;
        unsigned long c_no[7];
        //↑Edit colors
        
        int i=0,j=0;
        int score=0;
        int mainFlag=0;
        int jumpFlag=0;
        int jumpStartFlag=0;
        int bodyX=350,bodyY=150;
        int iconX=200,iconY=150,iconSize=2;
        int groundY=550;
        int jumpSpeed;
        int Gravity;
        int jumpMove;
        int jumpStart;
        int jumpEnd;
        int jumpDur; 
        //↑User-made variables

        dpy = XOpenDisplay(""); 
        //↑connect to the display server(DO NOT CHANGE)

        root = DefaultRootWindow (dpy);
        screen = DefaultScreen (dpy);
        white = WhitePixel (dpy, screen);
        black = BlackPixel (dpy, screen);
        cmap = DefaultColormap(dpy,0);
        //↑Configuration(DO NOT CHANGE)

        for(i=0;i<8;i++){
        XAllocNamedColor(dpy,cmap,color_name[i],&c1,&c0);
                c_no[i]=c1.pixel;
        }
        //↑Configure Colors

	base = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	exit = XCreateSimpleWindow(dpy, base, 10, 3, 30, 12, BORDER, black, white);
        //↑Edit windows

	gc = XCreateGC(dpy,base, 0, NULL);
        //↑DO NOT CHANGE

	XSelectInput(dpy,base,ButtonPressMask);
	XSelectInput(dpy,exit,ButtonPressMask);    
        //Edit events

        fs[0]=XCreateFontSet(dpy,"-*-lucida-*-*-*-*-26-*",&miss,&n_miss,&def);
        fs[1]=XCreateFontSet(dpy,"-*-fixed-*-*-*-*-10-*",&miss,&n_miss,&def);
        fs[2]=XCreateFontSet(dpy,"-*-fixed-*-*-*-*-16-*",&miss,&n_miss,&def);
        fs[3]=XCreateFontSet(dpy,"-*-fixed-*-*-*-*-24-*",&miss,&n_miss,&def);
        //↑Edit fonts

	XMapWindow(dpy, base);
	XMapSubwindows(dpy, base);
        //↑DO NOT CHANGE

        XSetForeground (dpy,gc,black);

	while(1){
		if(XEventsQueued(dpy, QueuedAfterReading)){
                        XNextEvent(dpy,&e);
                        switch(e.type){
                        case ButtonPress:
                                if(e.xany.window==exit){
                                        return 0;
                                }
                                else if(mainFlag==0){ //Click on title 
                                                mainFlag=1;
                                }
                                else if(jumpFlag==0 || jumpFlag==2||mainFlag==2){ //Click after jump / retry
                                        jumpFlag=1;
                                        mainFlag=1;
                                        jumpStartFlag=0;
                                }
                                XFlush(dpy);
                                break;}
                }
                else if(mainFlag==0){ //Title
        		XDrawArc(dpy,base,gc,iconX,iconY,80*iconSize,80*iconSize,0,360*32);
                	XDrawArc(dpy,base,gc,iconX+(20*iconSize),iconY,80*iconSize,80*iconSize,0,360*32);
                	XDrawArc(dpy,base,gc,iconX+(40*iconSize),iconY,80*iconSize,80*iconSize,0,360*32);
                	XDrawArc(dpy,base,gc,iconX+(60*iconSize),iconY,80*iconSize,80*iconSize,0,360*32);
                	XDrawArc(dpy,base,gc,iconX+(80*iconSize),iconY,80*iconSize,80*iconSize,0,360*32);
               		XDrawArc(dpy,base,gc,iconX+(100*iconSize),iconY,80*iconSize,80*iconSize,0,360*32);
                	XDrawArc(dpy,base,gc,iconX+(120*iconSize),iconY,80*iconSize,80*iconSize,0,360*32);
                	XDrawRectangle(dpy,base,gc,iconX+(40*iconSize),iconY+(80*iconSize),40*iconSize,20*iconSize);
                	XDrawRectangle(dpy,base,gc,iconX+(120*iconSize),iconY+(80*iconSize),40*iconSize,20*iconSize);
		        XDrawRectangle(dpy,base,gc,iconX+(10*iconSize),iconY+(14*iconSize),180*iconSize,160*iconSize);
		        XDrawLine(dpy,base, gc,iconX+(70*iconSize),iconY+(120*iconSize),iconX+(60*iconSize),iconY+(140*iconSize));
		        XDrawLine(dpy,base, gc,iconX+(90*iconSize),iconY+(120*iconSize),iconX+(90*iconSize),iconY+(140*iconSize));
		        XDrawLine(dpy,base, gc,iconX+(110*iconSize),iconY+(120*iconSize),iconX+(120*iconSize),iconY+(140*iconSize));
		        XDrawLine(dpy,base, gc,iconX+(130*iconSize),iconY+(120*iconSize),iconX+(150*iconSize),iconY+(140*iconSize));
                        sprintf(moji, "縄跳びで遊ぼう");
                                XmbDrawString(dpy,base, fs[3], gc,  320, 100, moji, strlen(moji));
                        sprintf(moji, "Exit");
                                XmbDrawString(dpy,exit, fs[1], gc,  4, 10, moji, strlen(moji));
                }
                else if(mainFlag==1){//Main game
                        XClearWindow(dpy, base); 
                        sprintf(moji, "Exit");
                                XmbDrawString(dpy,exit, fs[1], gc,  4, 10, moji, strlen(moji));
                        XDrawPoint(dpy,base,gc,e.xbutton.x,e.xbutton.y);
                        sprintf(moji, "Score:");
                                XmbDrawString(dpy,base, fs[2], gc,  650, 50, moji, strlen(moji));
                        sprintf(moji,"%d",score);
                                XmbDrawString(dpy,base, fs[2], gc,  725, 50, moji, strlen(moji));
                                if(jumpFlag>0){
                                i++;
                                XFillArc(dpy,base,gc,392-250*sin(i/100.0),285-250*cos(i/100.0),15,15,0,360*64);
                        }
                        if(jumpFlag==1){
                                if(jumpStartFlag==0){
                                        jumpStart=i;
                                        jumpStartFlag=1;   
                                }
                                jumpDur=i-jumpStart;  
                                j=jumpDur/65;
                                if(j>=18){
                                        jumpFlag=2;
                                        score++;
                                        sprintf(moji,"%d",score);
                                                XmbDrawString(dpy,base, fs[1], gc,  725, 50, moji, strlen(moji));
                                }
                                jumpSpeed=-(9*j);
                                Gravity=((1*j*j)/2);
                                jumpMove=jumpSpeed+Gravity;
                        }                    
                        XDrawArc(dpy,base,gc,bodyX,bodyY+jumpMove,100,100,0,360*64);
                        XFillArc(dpy,base,gc,bodyX+20,bodyY+50+jumpMove,10,10,0,360*64);
                        XDrawRectangle(dpy,base,gc,bodyX+10,bodyY+100+jumpMove,80,150);
                        XDrawRectangle(dpy,base,gc,bodyX+30,bodyY+250+jumpMove,40,150);
                        XDrawLine(dpy, base, gc,0,groundY,800,groundY);
                        XDrawLine(dpy, base, gc,bodyX+60,bodyY+140+jumpMove,bodyX+40,bodyY+130+jumpMove);
                        XDrawLine(dpy, base, gc,bodyX+40,bodyY+130+jumpMove,bodyX+5,bodyY+200+jumpMove);
                        XDrawLine(dpy, base, gc,bodyX+5,bodyY+200+jumpMove,bodyX+25,bodyY+210+jumpMove);
                        XDrawLine(dpy, base, gc,bodyX+25,bodyY+210+jumpMove,bodyX+60,bodyY+140+jumpMove);
                        usleep(1000);
                        XFlush(dpy);
                        if(392-250*sin(i/100.0) >= 380  &&  392-250*sin(i/100.0) <= 420  &&  285-250*cos(i/100.0) < groundY+jumpMove  &&  285-250*cos(i/100.0) > groundY+jumpMove-200){
                                mainFlag=2;
                        }
	        }
                else if(mainFlag==2){  //Game over
                        XDrawLine(dpy ,base,gc,bodyX+25,bodyY+38+jumpMove,bodyX+35,bodyY+43+jumpMove);
                        sprintf(moji, "GET OUTA HERE!");
                                XmbDrawString(dpy,base, fs[3], gc,  322, 100, moji, strlen(moji));
                        sprintf(moji, "Click to Retry");
                                XmbDrawString(dpy,base, fs[2], gc,  350, 570, moji, strlen(moji));
                        i=0;
                        score=0;
                }
        }
}
