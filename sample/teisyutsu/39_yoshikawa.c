/*　report.c　*/
#include <stdio.h>
#include <string.h>
#include <math.h>	/*　数学関数を使う　*/
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>

#define BORDER 2
#define WIDTH 1000
#define HIGHT 700

int  main(int argc, char **argv){
	Display *dpy;
	Window w, quit;
 	Window root;
 	int screen;
 	unsigned long black, white;

     int  j=0, k, xx, yy, flag=0; 
     float i, h=0, d, pi, kaku, v0, max;
     char moji1[256], moji2[256], moji3[256], moji4[256], show[256], zahyou[256];
     char height[256]={'\0'};
     pi=3.1416;  
     printf("θ=");
     scanf("%f",&kaku);
     printf("v0=");
     scanf("%f",&v0);

     GC gc;
     XEvent e;
     dpy = XOpenDisplay("");

	root = DefaultRootWindow(dpy);
	screen = DefaultScreen(dpy);
	white = WhitePixel(dpy, screen);
	black = BlackPixel(dpy, screen);

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
/*　サブウインドウを作る　*/
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);

	XSelectInput(dpy, w, ButtonPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);	/*　サブウインドウ用　*/

	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);	/*　サブウインドウも表示　*/

	XSetForeground(dpy, gc, black);


     while(1){
		if(XEventsQueued(dpy, QueuedAfterReading)){ /* for ground */
                     
			XNextEvent(dpy, &e);
			switch(e.type){

				case ButtonPress :
                                   if(e.xany.window==quit)return 0;
                                   if(e.xbutton.button==2)XClearWindow(dpy,w); //真ん中ボタンを押したらクリア
                    
                                   if(e.xbutton.button==1){ // 左クリックが押されたら
                                        h=0;
                                        i=0;
                                        while(i<15.0){
                                           h=400.0-((v0*i*sin((kaku*pi)/180.0))+((1.0/2.0)*(-9.8)*i*i)); // y=v0*t*sinθ-(1/2)*g*t*t 
                                           d=40.0+(v0*cos((kaku*pi)/180.0)*i);  //x=v0×cosθ×t    
                                           XFillArc(dpy,w,gc,d,h,20,20,0*64,360*64);
                                           i=i+0.5; 

                                           height[j]=h;
                                           j++;
					   max=height[0];
                                           for(k=1;k<30;k++){
                                             if(max<height[k]) max=height[k];
						}

                                              } 
                                          if(flag==0){                 
                                           sprintf(show,"the Highest Point = %4.1f ",400.0-max);   //最高点表示
                                           XDrawString(dpy,w,gc,WIDTH / 2, HIGHT / 2 ,show ,strlen(show));
                                           flag=1;        
   
                                            usleep(100000);    
                                            XFlush(dpy);
                                           }
                                     }
                                  
                                  flag=0;                        
                        }
                }else {                
                     XDrawString(dpy,quit,gc,4,10,"Exit",4);
                     XFillRectangle(dpy,w,gc,35,420,30,20); // 投げ上げ台表示
                      
                     sprintf(moji1,"left click : jump"); 
                     XDrawString(dpy,w,gc,60,15,moji1,strlen(moji1));
                     sprintf(moji2,"center click : clear"); 
                     XDrawString(dpy,w,gc,60,28,moji2,strlen(moji2)); 
 
                     sprintf(moji3,"kaku= %3.0f",kaku); 
                     XDrawString(dpy,w,gc,WIDTH/2 , HIGHT/2-20, moji3,strlen(moji3));                   
                     sprintf(moji4,"v0= %3.0f",v0); 
                     XDrawString(dpy,w,gc,WIDTH/2+80 , HIGHT/2-20, moji4,strlen(moji4));          
                    

           
                 }
         }
     }





