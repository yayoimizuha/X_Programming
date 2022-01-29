/* xdraw_sin.c */
#include <stdio.h>
#include <string.h>
#include <math.h>    /* use math lib*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>

#define BORDER 2
#define WIDTH  500
#define HIGHT 350
void Read();

float  A,f,isou;
int xxs,yys,xxc,yyc,xxt,yyt,rap2;

int main(int argc, char **argv)
{
	Display *dpy;
	Window w,quit,set,sinpu,syuuha,Isou;
	Window root;
	int    screen;
	
	/*printf(" 振幅(cm) = ");scanf("%f",&A);
	printf(" 周波数(Hz) = ");scanf("%f",&f);
	printf(" 位相(deg) = ");scanf("%f",&isou);*/
	Read();
	
	int atai=0;
	//isou+=360;
	
	unsigned long black, white,c_no[4];
	char color_name[4][10] = {"red","white","blue","black"};
	char moji[256]; 
	XFontSet fs;             /* XFontsetで日本語フォントを選ぶ（日） */
	char **miss,*def;
    int n_miss;
	GC       gc;
	XEvent   e;
	float rap=0;
	dpy = XOpenDisplay("");
	setlocale(LC_ALL,"");

	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);

		Colormap cmap; 
        XColor c0,c1; 
        cmap = DefaultColormap(dpy,0);  /* カラーマップを得る */

	for(int i=0;i<4;i++) {
		XAllocNamedColor(dpy,cmap,color_name[i],&c1,&c0);  /* c1.pixel */
		c_no[i]=c1.pixel;
	}

	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);
	/* Make Sub Window */
	quit = XCreateSimpleWindow(dpy, w, 10, 1, 35, 14, BORDER, black, white);
	set = XCreateSimpleWindow(dpy, w, 55, 1, 30, 14, BORDER, black, white);
	sinpu = XCreateSimpleWindow(dpy, w, 180, 1, 35, 14, BORDER, black, white);
	syuuha = XCreateSimpleWindow(dpy, w, 310, 1, 35, 14, BORDER, black, white);
	Isou = XCreateSimpleWindow(dpy, w, 410, 1, 35, 14, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);

	XSelectInput(dpy, w, ButtonPressMask | KeyPressMask );
	XSelectInput(dpy, quit, ButtonPressMask);
	XSelectInput(dpy, set, ButtonPressMask);
	XSelectInput(dpy, syuuha, ButtonPressMask);
	XSelectInput(dpy, sinpu, ButtonPressMask);
	XSelectInput(dpy, Isou, ButtonPressMask);


	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);
	
	fs=XCreateFontSet(dpy,"-*-*-medium-r-normal-*-16-*",&miss,&n_miss,&def);
	rap2 = 1;
	
	while(1){
	   XDrawString(dpy,quit,gc,3,12,"Exit",4);
	   XDrawString(dpy,set,gc,3,12,"set",3);
	   sprintf(moji,"%2.0f",f);
	   XDrawString(dpy,syuuha,gc,3,12,moji,strlen(moji));
	   sprintf(moji,"%2.0f",A);
	   XDrawString(dpy,sinpu,gc,3,12,moji,strlen(moji));
	   sprintf(moji,"%2.0f",isou);
	   XDrawString(dpy,Isou,gc,3,12,moji,strlen(moji));
	   XDrawString(dpy,w,gc,100,16,"frequency",9);
	   XDrawString(dpy,w,gc,230,16,"amplitude",9);
	   XDrawString(dpy,w,gc,360,16,"phase",5);
	   XNextEvent(dpy, &e);
	   switch(e.type){
	   case ButtonPress : 
	   		if(e.xany.window == quit)return 0;
	   		else if(e.xany.window == set){
	   			atai=0;
				Read();
				rap = 0;
				rap2=1;
				XClearWindow(dpy,w);
			}
			else if(e.xany.window == syuuha){
				atai=1;
				XClearWindow(dpy,syuuha);
				f=0;
			}
			else if(e.xany.window == sinpu){
				atai=2;
				XClearWindow(dpy,sinpu);
				A=0;
			}
			else if(e.xany.window == Isou){
				atai=3;
				XClearWindow(dpy,Isou);
				isou=0;
			}
	   break;

	   case KeyPress : {
	   		int ii ;
	   		if(e.xkey.keycode<=19 && e.xkey.keycode>=10){
	   			XClearWindow(dpy,syuuha);
				XClearWindow(dpy,sinpu);
				XClearWindow(dpy,Isou);
	   			ii = (e.xkey.keycode - 9)%10;
	   			if(rap2==1){
					if(atai == 1)f+=ii;
					if(atai == 2)A+=ii;
					if(atai == 3)isou+=ii;
				}
				else if(rap2==2){
					if(atai == 1)f+=ii*10;
					if(atai == 2)A+=ii*10;
					if(atai == 3)isou+=ii*10;
				}
				else if(rap2==3){
					if(atai == 1)f+=ii*100;
					if(atai == 2)A+=ii*100;
					if(atai == 3)isou+=ii*100;
				}
				rap2++;
			}
	   		break;
	   		}
	   }
	   if(rap == 0){
	   	  		
	      		XDrawLine(dpy, w, gc, 50,175,450,175);
	      		XDrawLine(dpy, w, gc, 50,50,50,300);
	      		for(int j=0,k=1;j<350;j+=90,k++){
					sprintf(moji,"%2.1f",k/2.0);
          		XmbDrawString(dpy, w, fs, gc, 50+82+j,205,moji,strlen(moji));
          		}
          	for(int j=55,k=120;k>=-120;j+=20,k-=20){
				 	sprintf(moji,"%d",k);
         	 	 	XmbDrawString(dpy, w, fs, gc, 10,j+5,moji,strlen(moji));
          		}
          	sprintf(moji,"[s]");
          	XmbDrawString(dpy, w, fs, gc, 440,205,moji,strlen(moji));
    	    	sprintf(moji,"[cm]");
        		XmbDrawString(dpy, w, fs, gc, 10,40,moji,strlen(moji));
          
	    		for(int j=90;j<400;j+=90)XDrawLine(dpy, w, gc, 50+j,165,50+j,185);
	   	  		for(int j=0;j<250;j+=20)XDrawLine(dpy, w, gc, 45,55+j,50,55+j);
	   	   		isou += 360;
         		for(int i=0;i<400;i++){
          		XDrawLine(dpy,w,gc,xxs,yys,50+i,175-A*sin((i+isou/2.0)*3.14/90.00*f));
           		xxs = 50+i; yys = 175-A*sin((i+isou/2.0)*3.14/90.00*f);
	        		XDrawLine(dpy,w,gc,xxc,yyc,50+i,175-A*cos((i+isou/2.0)*3.14/90.00*f));
	        		xxc = 50+i; yyc = 175-A*cos((i+isou/2.0)*3.14/90.00*f);
	     	   		if(yyt>(175-A*tan((i+isou/2.0)*3.14/90.00*f)))
	     	   		XDrawLine(dpy,w,gc,xxt,yyt,50+i,175-A*tan((i+isou/2.0)*3.14/90.00*f));
	     	   		XSetForeground (dpy,gc,c_no[1]); 
					if(yyt<(175-A*tan((i+isou/2.0)*3.14/90.00*f)) && yyt)
	     	   			XDrawLine(dpy,w,gc,xxt,yyt,50+i,175-A*tan((i+isou/2.0)*3.14/90.00*f));
					XSetForeground (dpy,gc,c_no[3]);
					xxt = 50+i; 
					yyt = 175-A*tan((i+isou/2.0)*3.14/90.00*f);		
	     		}
	     		isou-=360;
	     		rap=1;
	     	}
	}
}

void Read(){
	xxs=50;
	xxc=50;
	xxt=50;
	yys=175-A*sin((isou/2.0)*3.14/90.00*f);
	yyc=175-A*cos((isou/2.0)*3.14/90.00*f);
	yyt=175-A*tan((isou/2.0)*3.14/90.00*f);
}

