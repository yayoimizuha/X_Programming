/*31_n-order.c*/
/*後半の方は読みづらいです*/
#include <stdio.h>
#include <math.h>	/*　数学関数を使う　*/
#include <unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define BORDER 2
#define WIDTH 1000
#define HIGHT 700

int  main(int argc, char **argv){
	Display *dpy;
	Window w, quit;
 	Window root;
 	int screen;
 	unsigned long black, white;
 	GC gc;
 	XEvent e;
	int i,j,x, y,n,flag;
	float value;
	float coefficient[100]={1};

//coefficient-係数


//n次方程式作成

	printf("n次方程式を作成\n");
	printf("1<=n<=100の実数を入力\nn:");
	scanf("%d",&n);


	if(n<1 || 100<n){
		printf("さようなら");
		return 0;
	}

	printf("%d次方程式を作成します。\n",n);

	printf("%d次の係数（0以外）を入力:",n);
	scanf("%f",&coefficient[n]);
	if(coefficient[n]==0){
		printf("さようなら");
		return 0;
		}

	for(i=n-1;0<=i;i--){
		printf("%d次の係数を入力:",i);
		scanf("%f",&coefficient[i]);
		if(i==1){
			i=0;
			printf("%d次の値を入力:",i);
			scanf("%f",&coefficient[i]);
		}
	}



//作成した方程式を表示

	printf("作成した方程式(見やすいように小数点一位まで表示しています。)\ny=");
	for(i=n;0<=i;i--){
		printf("%3.2f",coefficient[i]);
		for(j=i-1;0<=j;j--)
			printf("*x");
		if(i!=0 && j!=0)
		printf("+");
	}
	printf("\n");

	

	printf("xの値を入力:");
	scanf("%f",&value);

	float y2=coefficient[n];

		for(j=n;1<=j;j--)
			y2=y2*value+coefficient[j-1];
		
	printf("%f(x=%0.2fのとき)\n",y2,value);

	float Y,Y2;
	Y=coefficient[n]*n;
	for(j=n-1;1<=j;j--)
		Y=Y*value+coefficient[j]*j;
	printf("傾き:%f(x=%0.2fのとき)\n",Y,value);

	char NOWY2[10],NOWX2[10];
	sprintf(NOWX2,"%f",value);
	sprintf(NOWY2,"%f",Y);

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
	Y2=Y;
	while(1){
		if(XEventsQueued(dpy, QueuedAfterReading)){ /* for ground */
			XNextEvent(dpy, &e);
			switch(e.type){
				case ButtonPress :
					if(e.xany.window == quit)return 0;
					flag = e.xbutton.button;
					if(flag == 1 )value=value+0.01;
					if(flag == 3 )value=value-0.01;
					for(j=n-1;1<=j;j--);
					Y2=Y2*value+coefficient[j]*j;
					sprintf(NOWY2,"%f",Y2);
					sprintf(NOWX2,"%f",value);
					XClearWindow(dpy, w);
			flag=0;
			}
		}else{	/* back ground */
			XDrawString(dpy, quit, gc, 4, 10, "Exit", 4);
			XDrawString(dpy, w, gc, 260, 20,"slope=" , 6);
			XDrawString(dpy, w, gc, 300, 20, NOWY2, 8);
			XDrawString(dpy, w, gc, 180, 20,"x=" , 2);
			XDrawString(dpy, w, gc, 200, 20, NOWX2, 8);
			XDrawLine(dpy, w, gc, 50, 300, 950, 300);	/* x軸 */
			XDrawLine(dpy, w, gc, 500, 50, 500, 650);		/* y軸 */
			for(i=-3000;i<2000;i++)
			XDrawPoint(dpy, w, gc, 500+i/10.0, 300-(Y2*i/10.0));	/* 点 */
			usleep(1000);
			XFlush(dpy);
		}
	}
}
