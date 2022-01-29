/* xdraw.c */
#include <stdio.h>
#include <string.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <unistd.h>
#define BORDER 2
#define WIDTH  615
#define HIGHT 460
#define pai 3.14//159265358979323846264338327950288419716939937510

int main(int argc,char **argv)
{
   int i= 0,xx,yy,x2,y2,Flag;
   float mokux,mokuy,doux,douy,tenx,teny,kaix,kaiy,meix,meiy,suix,suiy,kinx,kiny,tikix,tikiy,kax,kay,mokutime = 0,doutime = 0,tentime = 0,kaitime = 0,meitime=0,suitime = 0,kintime = 0,tikitime = 0,katime = 0,speed=10500;
   	mokux = 195;
	Display *dpy;
	Window  w;
	Window  root;
	Window  quit;
	Window  quit2;
	Window  button;
	Window  button2;
	Window  button4;
	Window  button05;
	Window  change;
	Window  Innerplanet;
	Window  Explanet;
	int     screen;
	unsigned long black, white;
	XSetWindowAttributes attr;
	GC      gc;
   XEvent  e;
   

   Colormap cmap;  /* カラーマップ構造体 */
	XColor c0[8],c1[8];     /* カラー構造体 */
	char color_name[8][20]={"red","green","blue","pink","yellow","black","white","gold"};
	
	dpy = XOpenDisplay("");
	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);

	cmap = DefaultColormap(dpy,0);  /* カラーマップを得る */
	for(i=0;i<8;i++)
      XAllocNamedColor(dpy,cmap,color_name[i],&c1[i],&c0[i]); /* c1.pixel */

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, black);
	
	//ボタン作成---------------------------------------------------------------------------------------------------------
	button05 = XCreateSimpleWindow(dpy,w,500,240,100,50,1,c1[0].pixel,WhitePixel(dpy,screen)); //0.5倍速
	button   = XCreateSimpleWindow(dpy,w,500,293,100,50,1,c1[0].pixel,WhitePixel(dpy,screen)); //1倍速
	button2  = XCreateSimpleWindow(dpy,w,500,346,100,50,1,c1[0].pixel,WhitePixel(dpy,screen)); //2倍速
	button4  = XCreateSimpleWindow(dpy,w,500,399,100,50,1,c1[0].pixel,WhitePixel(dpy,screen)); //4倍速
	quit     = XCreateSimpleWindow(dpy,w,10,10,27,10,1,c1[0].pixel,WhitePixel(dpy,screen)); //EXIT
	change   = XCreateSimpleWindow(dpy,w,500,187,100,50,1,c1[0].pixel,WhitePixel(dpy,screen)); 
	Innerplanet = XCreateSimpleWindow(dpy,root,0,0,200,260,1,black,black);
	Explanet    = XCreateSimpleWindow(dpy,root,10,10,20,20,1,c1[0].pixel,WhitePixel(dpy,screen));
	//マップして表示
    XMapWindow(dpy,button);
    XMapWindow(dpy,button2);
    XMapWindow(dpy,button4);
    XMapWindow(dpy,button05);
    XMapWindow(dpy,quit);
    XMapWindow(dpy,change);
    //イベントマスクを登録
    XSelectInput(dpy,button,ButtonPressMask | ButtonReleaseMask);
    XSelectInput(dpy,button2,ButtonPressMask | ButtonReleaseMask);
    XSelectInput(dpy,button4,ButtonPressMask | ButtonReleaseMask);
    XSelectInput(dpy,button05,ButtonPressMask | ButtonReleaseMask);
    XSelectInput(dpy,quit,ButtonPressMask | ButtonReleaseMask);
    XSelectInput(dpy,change,ButtonPressMask | ButtonReleaseMask);
    XSelectInput(dpy,Explanet,ButtonPressMask | ButtonReleaseMask);
    //-------------------------------------------------------------------------------------------------------------------
    
    //ウィンドウの内容を記録
    attr.backing_store = WhenMapped;
    XChangeWindowAttributes(dpy,w,CWBackingStore,&attr);
    
    //ウィンドウ名の設定
    XStoreName(dpy,w,"Explanet");
    XSetIconName(dpy,w,"Explanet");
    XStoreName(dpy,Innerplanet,"Inner planet");
    XSetIconName(dpy,Innerplanet,"Inner planet");
    
    	
	//グラフィックコンテキストを取得
	gc = XCreateGC(dpy, w, 0, NULL);
	XEvent event;
	//イベントマスクを登録
   XSelectInput(dpy, w, ExposureMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask);
   //マップして表示
	XMapWindow(dpy, w);
	XMapSubwindows(dpy,w);
	
	XFlush(dpy);
	
	 /*char str[255];
	                     sprintf(str,"Mars planet");
	                     int width = 20;
	                     int height = 30;
	                     XDrawString(dpy,button,gc,width,height,str,strlen(str));      
	                     XFlush(dpy);*/
		
	while(1){
   XClearWindow(dpy,w);
   XClearWindow(dpy,Innerplanet);
   XSetForeground(dpy, gc, c1[2].pixel);
   XDrawString(dpy,change,gc,18,30,"InnerPlanet",11);
   XDrawString(dpy,button,gc,12,30,"x1 1year/min",12);
   XDrawString(dpy,button2,gc,12,30,"x2 2year/min",12);
   XDrawString(dpy,button4,gc,12,30,"x4 4year/min",12);
   XDrawString(dpy,button05,gc,3,30,"x0.5 0.5year/min",16);
   XDrawString(dpy,quit,gc,2,10,"EXIT",4);
   if(XEventsQueued(dpy, QueuedAfterReading)){                       //イベントがないとき
	   XNextEvent(dpy,&event);
	   switch(event.type){
	      case ButtonPress:                                           //画面上でマウスのボタンが押されたとき
	         if(button==event.xany.window){
	            XSetWindowBorderWidth(dpy,event.xany.window,2);       //ボーダーラインを太くする
	            speed = 10500;
	            }else if(button2==event.xany.window){
	            XSetWindowBorderWidth(dpy,event.xany.window,2);       //ボーダーラインを太くする
	            speed = 5250;
	            }else if(button4==event.xany.window){
	            XSetWindowBorderWidth(dpy,event.xany.window,2);       //ボーダーラインを太くする
	            speed = 2500;
	            }else if(button05==event.xany.window){
	            XSetWindowBorderWidth(dpy,event.xany.window,2);       //ボーダーラインを太くする
	            speed = 100000;
	            }else if(quit==event.xany.window){
	            XSetWindowBorderWidth(dpy,event.xany.window,2);       //ボーダーラインを太くする
	            return 0;
	            }else if(change==event.xany.window){
	            XSetWindowBorderWidth(dpy,event.xany.window,2);       //ボーダーラインを太くする
	            XMapWindow(dpy,Innerplanet);                          //内惑星用に切り替え
	            }else if(Explanet==event.xany.window){
	            XSetWindowBorderWidth(dpy,event.xany.window,2);       //ボーダーラインを太くする
	            Innerplanet = 0;
	            }
            break;
	      case ButtonRelease:                                         //画面上でマウスのボタンが話されたとき
	         if(button==event.xany.window){                           //ボタンのイベントかどうかを判定
	            XSetWindowBorderWidth(dpy,event.xany.window, 1);      //ボーダーラインを細くする
              }else if(button2==event.xany.window){                  //ボタンのイベントかどうかを判定
	            XSetWindowBorderWidth(dpy,event.xany.window, 1);      //ボーダーラインを細くする
	           }else if(button4==event.xany.window){                  //ボタンのイベントかどうかを判定
	            XSetWindowBorderWidth(dpy,event.xany.window, 1);      //ボーダーラインを細くする
	           }else if(button05==event.xany.window){                  //ボタンのイベントかどうかを判定
	            XSetWindowBorderWidth(dpy,event.xany.window, 1);      //ボーダーラインを細くする
	           }
	         XFlush(dpy);
	         break;
	      default:
	         break;
	    }
	         
	
	
	//↓変更前メモ
	/*XSetForeground(dpy, gc, c1[0].pixel);
	XFillArc(dpy,w,gc,198,343,4,4,0,360*64);
	XSetForeground(dpy, gc, c1[5].pixel);
	XDrawArc(dpy,w,gc,195,340,10,10,0,360*64);   //太陽系
	XDrawArc(dpy,w,gc,185,330,30,30,0,360*64);   //木星
	XDrawArc(dpy,w,gc,175,320,50,53,0,360*64);   //土星
	XDrawArc(dpy,w,gc,150,295,100,100,0,360*64); //天王星
	XDrawArc(dpy,w,gc,120,265,160,160,0,360*64); //海王星
	XDrawArc(dpy,w,gc,115,220,210,210,0,360*64); //冥王星
	XFillArc(dpy,w,gc,mokux,mokuy,2,2,0,360*64); //木星
	XFillArc(dpy,w,gc,doux,douy,2,2,0,360*64); //土星
	XFillArc(dpy,w,gc,tenx,teny,2,2,0,360*64); //天王星
	XFillArc(dpy,w,gc,kaix,kaiy,2,2,0,360*64); //海王星
	XFillArc(dpy,w,gc,meix,meiy,2,2,0,360*64); //冥王星*/
	
	}else{
		//惑星の周回及び周回速度調整---------------------------------------------------------------------------------------------------------------
		/*木星*/
	   mokux=198+15*sin(mokutime);
	   mokuy=343+15*cos(mokutime);
	   mokutime += ((15*2.0*pai)/(1/*年*/*24/*時間*/*60/*分*/*60/*秒*/));//11.8
	   /*土星*/
	   doux=198+25*sin(doutime);
	   douy=345.5+27.5*cos(doutime);
	   doutime += ((25*2.0*pai)/(29.5/*年*/*24/*時間*/*60/*分*/*60/*秒*/));
	   /*天王星*/
	   tenx=198+50*sin(tentime);
	   teny=343+50*cos(tentime);
	   tentime += ((50*2.0*pai)/(84.2/*年*/*24/*時間*/*60/*分*/*60/*秒*/));
	   /*海王星*/
	   kaix=198+80*sin(kaitime);
	   kaiy=343+80*cos(kaitime);
	   kaitime += ((80*2.0*pai)/(164.7/*年*/*24/*時間*/*60/*分*/*60/*秒*/));
	   /*冥王星*/
	   meix=218+105*sin(meitime);
	   meiy=323+105*cos(meitime);
	   meitime += ((105*2.0*pai)/(247.7/*年*/*24/*時間*/*60/*分*/*60/*秒*/));
	   /*水星*/
	   suix=98+20*sin(suitime);
	   suiy=143+20*cos(suitime);
	   suitime += ((20*2.0*pai)/(0.241/*年*/*24/*時間*/*60/*分*/*60/*秒*/));
	   /*金星*/
	   kinx=98+30*sin(kintime);
	   kiny=143+30*cos(kintime);
	   kintime += ((30*2.0*pai)/(0.616/*年*/*24/*時間*/*60/*分*/*60/*秒*/));
	   /*地球*/
	   tikix=98+38*sin(tikitime);
	   tikiy=143+38*cos(tikitime);
	   tikitime += ((38*2.0*pai)/(1/*年*/*24/*時間*/*60/*分*/*60/*秒*/));
	   /*火星*/
	   kax=93+65*sin(katime);
	   kay=143+65*cos(katime);
	   katime += ((65*2.0*pai)/(1.882/*年*/*24/*時間*/*60/*分*/*60/*秒*/));
	   //---------------------------------------------------------------------------------------------------------------------------------------------
	   XSetForeground(dpy, gc, c1[0].pixel);
		XFillArc(dpy,w,gc,198,343,4,4,0,360*64);
		XSetForeground(dpy, gc, c1[6].pixel);
		XDrawArc(dpy,w,gc,195,340,10,10,0,360*64);   //太陽系
		XDrawArc(dpy,w,gc,185,330,30,30,0,360*64);   //木星
		XDrawArc(dpy,w,gc,175,320,50,53,0,360*64);   //土星
		XDrawArc(dpy,w,gc,150,295,100,100,0,360*64); //天王星
		XDrawArc(dpy,w,gc,120,265,160,160,0,360*64); //海王星
		XDrawArc(dpy,w,gc,115,220,210,210,0,360*64); //冥王星
		XSetForeground(dpy, gc, c1[4].pixel);
		XFillArc(dpy,w,gc,mokux,mokuy,4,4,0,360*64); //木星
		XFillArc(dpy,w,gc,doux,douy,4,4,0,360*64); //土星
		XFillArc(dpy,w,gc,tenx,teny,4,4,0,360*64); //天王星
		XFillArc(dpy,w,gc,kaix,kaiy,4,4,0,360*64); //海王星
		XFillArc(dpy,w,gc,meix,meiy,4,4,0,360*64); //冥王星
		//---------------------------------------------------------------------------------------------------------------------------------------------
		XSetForeground(dpy, gc, c1[0].pixel);
		XFillArc(dpy,Innerplanet,gc,95,140,10,10,0,360*64);
		XSetForeground(dpy, gc, c1[6].pixel);
		XDrawArc(dpy,Innerplanet,gc,80,125,40,40,0,360*64);   //水星
		XDrawArc(dpy,Innerplanet,gc,70,115,60,60,0,360*64);   //金星
		XDrawArc(dpy,Innerplanet,gc,62,107,76,76,0,360*64);   //地球
		XDrawArc(dpy,Innerplanet,gc,30,80,130,130,0,360*64); //火星
	   XSetForeground(dpy, gc, c1[4].pixel);
		XFillArc(dpy,Innerplanet,gc,suix,suiy,4,4,0,360*64); //水星
		XFillArc(dpy,Innerplanet,gc,kinx,kiny,4,4,0,360*64); //金星
		XFillArc(dpy,Innerplanet,gc,tikix,tikiy,4,4,0,360*64); //地球
		XFillArc(dpy,Innerplanet,gc,kax,kay,4,4,0,360*64); //火星
		XFlush(dpy);
		usleep(speed);//10500
	

	
	
	
	/*
	while(1){
	   XNextEvent(dpy,&e);
	   switch (e.type){
         case ButtonPress :
           if(e.xbutton.button==1){
            xx=e.xbutton.x;
            yy=e.xbutton.y;
            printf("x=%d y=%d button=%d \n",e.xbutton.x,e.xbutton.y,e.xbutton.button);
            }else if(e.xbutton.button==2){
            x2=e.xbutton.x;
            y2=e.xbutton.y;
}
		 break;
	      case ButtonRelease :
	         if(e.xbutton.button==1){
            XDrawLine(dpy, w, gc, xx, yy, e.xbutton.x, e.xbutton.y);
            }else if(e.xbutton.button==2){
            XDrawRectangle(dpy, w, gc, x2, y2, e.xbutton.x, e.xbutton.y);
            }
	   }
	}
}
*/
}
   /*XSetForeground(dpy, gc, c1[2].pixel);
	XDrawString(dpy,quit,gc,4,10,"Exit",4);
	XNextEvent(dpy,&e);
		switch(e.type){
			case ButtonPress :
			if(e.xany.window == quit)
					return 0;
		    break;*/
}
}
