/* kadai.c */
#include <stdio.h>
#include <math.h>	/*　数学関数を使う　*/
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define BORDER 2
#define WIDTH 1000
#define HIGHT 800
#define PI 3.141592

int	main(int argc, char **argv){
	Display *dpy;
	Window w, quit, ground;
	Window root;
	int screen;
	unsigned long black, white;
	GC gc;
	XEvent e;
	int i;
	Colormap cmap;		/* カラーマップ構造体 */
	XColor c0,c1;		/* カラー構造体 */
	char color_name[8][20]={"red","green","blue","pink","yellow","violet","black","white"};
	unsigned long c_no[8];
	int a;
	
	
	float bs, cor, tt;				//bsボールのサイズ cor反発係数 tt継続時間 


	printf("ボールをマウスでタップした方向に飛ばします。\nボールから見て右上あたりをクリックしてください。\n");
	printf("ボールのサイズを入力してください(だいたい20〜200)\n");
	printf("直径\n");
	scanf("%f",&bs);
	printf("反発係数を入力してください(あまり大きいとすぐにどっかに行きます)\n");
	scanf("%f",&cor);
	printf("継続時間を入力してください(10で1秒くらい)\n");
	scanf("%f",&tt);

	dpy = XOpenDisplay("");
	root = DefaultRootWindow(dpy);
	screen = DefaultScreen(dpy);
	white = WhitePixel(dpy, screen);
	black = BlackPixel(dpy, screen);
	cmap = DefaultColormap(dpy,0);			/* カラーマップを得る */
	
	
	for(a=0;a<8;a++){
		XAllocNamedColor(dpy,cmap,color_name[a], &c0,&c0);
		c_no[a]=c0.pixel;
		//printf("%ld\n", c_no[a]);
	}
	
	
	
	
	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	/*　サブウインドウを作る　*/
	quit = XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);
	XSelectInput(dpy, w, ButtonPressMask);
	XSelectInput(dpy, quit, ButtonPressMask);	/*　サブウインドウ用　*/
	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);				/*　サブウインドウも表示　*/
	XSetForeground(dpy, gc, black);





	while(1){
		//マウスの位置
		XNextEvent(dpy,&e);
		switch (e.type){
			case ButtonPress :
			break;
		case ButtonRelease :
			XDrawPoint(dpy,w,gc,e.xbutton.x,e.xbutton.y);
		}

		float vx, vy, g=9.8, mx=e.xbutton.x, my=e.xbutton.y, t, sita, tx, ty, kx=800.0, gy=700.0;
		//vx初速度 mx,myマウス x,y座標 xのt時間 sita角度 tx,ty動かないボールの位置(定点) gy地面の高さ

		tx=bs/2.0;
		ty=gy-bs/2.0;

		vx=(mx-tx)/4.0;
		vy=(ty-my)/4.0;

		sita=atan((ty-my)/(mx-tx));

		int flag=0;
		float yt=0.0, xt=0.0, xx=1, x, y;	
		for(t=0;t<tt;t=t+0.1){
			printf("%f	%f\n",x,y);		//今の座標
			if(y>gy-bs/2.0){			//地面
				yt=0;
				xx=xx*cor;
			}
			
			if(x>800.0-bs/2.0){	//右の壁
				flag=1;
			}
	
			if(x<bs/2.0){		//左の壁
				flag=0;
			}
	
			if(flag==0){
				xt=xt+0.1*xx;
				x=vx*cos(sita)*xt+tx;			//斜方投射 xt x用の時間
				y=(g*yt*yt/2.0-vy*sin(sita)*yt)*xx+gy-bs/2.0;	//yt　y用の時間
			}
			
			if(flag==1){
				xt=xt-0.1*xx;
				x=vx*cos(sita)*xt+tx;			//斜方投射 xt x用の時間
				y=(g*yt*yt/2.0-vy*sin(sita)*yt)*xx+gy-bs/2.0;	//yt　y用の時間
			}
			
			if(XEventsQueued(dpy, QueuedAfterReading)){	/* for ground */
				XNextEvent(dpy, &e);
				switch(e.type){
					case ButtonPress :
				if(e.xany.window == quit)return 0;
					i=0;
				XClearWindow(dpy, w);
				}
			}
			else{/* back ground */
			XClearWindow(dpy, w);//消すやつ
			
			XSetForeground (dpy,gc,c_no[6]);
			XFillRectangle(dpy,w,gc,0,0,WIDTH,HIGHT);
			
			
			XSetForeground (dpy,gc,c_no[5]);
			
			XFillArc(dpy,w,gc,x-bs/2.0,y-bs/2.0,bs,bs,0,360*64);//動くボール
			
			XSetForeground (dpy,gc,c_no[6]);
			
			XDrawString(dpy, quit, gc, 4, 10, "Exit", 4);
			
			XSetForeground (dpy, gc, c_no[7]);
			
			XDrawLine(dpy, w, gc, 0, gy, WIDTH, gy);//地面
			XDrawLine(dpy, w, gc, kx, 0, kx, HIGHT);//右の壁
			XDrawLine(dpy, w, gc, tx, ty, mx, my);//定点とマウスを結んだ線
			
				XSetForeground (dpy, gc, c_no[1]);		

			XFillArc(dpy, w, gc, tx-bs/2.0, gy-bs, bs, bs, 0, 360*64); //動かないボール
			yt=yt+0.1;
			}
			usleep(10000);
			XFlush(dpy);
		}
		printf("発射した角度%f \n",sita*180.0/PI); 
		printf("x方向の初速%f \n",vx);
		printf("y方向の初速%f \n",vy);
		printf("クリックした座標 押したボタンの種類\nx=%d y=%d      button=%dp \n\n", e.xbutton.x, e.xbutton.y, e.xbutton.button);
	}
}

