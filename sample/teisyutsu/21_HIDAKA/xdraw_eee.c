#include "ttt.h"

#define colorval 8
#define OKISA 250

int main(int argc,char **argv){
//セットアップ
	setlocale(LC_ALL,"");
	dpy = XOpenDisplay("");
	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	black=BlackPixel(dpy, screen);
	white=WhitePixel (dpy, screen);
	cmap=DefaultColormap(dpy,0);//カラーマップを得る
	XColor ce;
	XColor c[colorval]={};//カラー構造体
	int i;
	char colorname[colorval][8]={"red","blue","green","yellow","pink","orange","purple","cyan"};
	for(i=0;i<colorval;i++)XAllocNamedColor(dpy,cmap,colorname[i],&c[i],&ce);//cl.pixal

	Vector2 pp,pplast,Wsize,Wpoint,Qsize,Qpoint;
	Wpoint.x=120;//窓展開の開始位置
	Wpoint.y=100;
	Wsize.x =1000;//窓サイズ
	Wsize.y =800;
	Qpoint.x=  10;//窓展開の開始位置
	Qpoint.y=    3;
	Qsize.x =  30;//窓サイズ
	Qsize.y =  12;
	
	w = W(Wpoint,Wsize);//窓の宣言
	quit = subW(w,Qpoint,Qsize);//サブ窓の宣言
	gc = XCreateGC(dpy, w, 0, NULL);
	
/*	setup(dpy,w);//窓表示
	setup(dpy,quit);//サブ窓表示*/
	XSelectInput(dpy, w, PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask);//読み込みマスク省略
	XSelectInput(dpy, quit, PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask);
	XMapWindow(dpy,w);
	XMapSubwindows(dpy,w);


//動作部分
	int Flag=0,colornow=1,movex=0,movey=0,u1=0,u2=0;
	float tx1,tx2,ty1,ty2,q[8];
	Vector2 t[8]={};
	while(1){
	XSetForeground(dpy, gc, black);
	XDrawString(dpy, quit, gc, 4, 10, "Exit", 4);
	
	XNextEvent(dpy,&e);
	switch (e.type){
		case ButtonPress :
			if(e.xany.window==quit)	return 0;//Exit窓クリックで脱出
			Flag=1;
			pp.x = e.xbutton.x;
			pp.y = e.xbutton.y;
			break;
		case ButtonRelease:
			Flag=0;
			pplast.x = e.xbutton.x;
			pplast.y = e.xbutton.y;
			break;
		case MotionNotify :
			if(Flag){
			movex=(pp.x-e.xbutton.x)*0.3;
			movey=(pp.y-e.xbutton.y)*0.5;
			XClearWindow(dpy, w);
			tx1=OKISA*sin(3.1415*(135+movex)/180);
			tx2=OKISA*sin(3.1415*(45+movex)/180);
			ty1=OKISA*sin(3.1415*(135+movey)/180);
			ty2=OKISA*sin(3.1415*(45+movey)/180);
			
			//省略失敗部分
			setplace(&t[0].x,&t[0].y,(Wsize.x/2)+tx1,(Wsize.y/2)+ty1);
			setplace(&t[1].x,&t[1].y,(Wsize.x/2)+tx2,(Wsize.y/2)+ty2);
			setplace(&t[2].x,&t[2].y,(Wsize.x/2)-tx1,(Wsize.y/2)-ty1);
			setplace(&t[3].x,&t[3].y,(Wsize.x/2)-tx2,(Wsize.y/2)-ty2);
			setplace(&t[4].x,&t[4].y,(Wsize.x/2)+tx1,(Wsize.y/2)+ty2);
			setplace(&t[5].x,&t[5].y,(Wsize.x/2)+tx2,(Wsize.y/2)+ty1);
			setplace(&t[6].x,&t[6].y,(Wsize.x/2)-tx1,(Wsize.y/2)-ty2);
			setplace(&t[7].x,&t[7].y,(Wsize.x/2)-tx2,(Wsize.y/2)-ty1);
			q[0]=20+(tx1+ty1)/25;
			q[1]=20+(tx2+ty2)/25;
			q[2]=20+(-tx1-ty1)/25;
			q[3]=20+(-tx2-ty2)/25;
			q[4]=20+(tx1+ty2)/25;
			q[5]=20+(tx2+ty1)/25;
			q[6]=20+(-tx1-ty2)/25;
			q[7]=20+(-tx2-ty1)/25;
			//ここまで
			
			for(i=0;i<8;i++){
			//試行錯誤の結果
			/*u1=(i%2<1)?tx1:tx2;
			u2=(i==0||2||5||7)?ty1:ty2;
			if((i%4)>=2){u1=-u1;u2=-u2;}
			setplace(&t[i].x,&t[i].y,(Wsize.x/2)+u1,(Wsize.y/2)+u2);
			q[i]=20+(u1+u2)/25;*/
			XSetForeground(dpy, gc, c[i].pixel);
			XFillArc(dpy,w,gc,t[i].x,t[i].y,q[i],q[i],0,360*64);
			}
			//それっぽい線
			XDrawLine(dpy,w,gc,500+tx1,400+ty1,500+tx2,400+ty2);
			XDrawLine(dpy,w,gc,500+tx2,400+ty2,500 -tx1,400 -ty1);
			XDrawLine(dpy,w,gc,500 -tx1,400 -ty1,500 -tx2,400 -ty2);
			XDrawLine(dpy,w,gc,500 -tx2,400 -ty2,500+tx1,400+ty2);
			XDrawLine(dpy,w,gc,500+tx1,400+ty2,500+tx2,400+ty1);
			XDrawLine(dpy,w,gc,500+tx2,400+ty1,500 -tx1,400 -ty2);
			XDrawLine(dpy,w,gc,500 -tx1,400 -ty2,500 -tx2,400 -ty1);
			XDrawLine(dpy,w,gc,500 -tx2,400 -ty1,500+tx1,400+ty1);
				}
				break;
	   	}
	}
					
}
