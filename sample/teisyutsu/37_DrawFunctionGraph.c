#include <stdio.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>

#define BORDER 2
#define WIDTH  800
#define HIGHT 800

typedef struct {
    int x;
    double y;
} coordinate;


int main(int argc, char **argv)
{
	Display *dpy;
	Window w, quit ,reset, draw, msg, ok;
	Window root;
	int    screen, count=0, i, j, k, y_value;
	double  coe_matrix[256][256], sol_matrix[256],  l;
    coordinate crd[256];
	unsigned long black, white;
	GC       gc;
	XEvent  e;

	dpy = XOpenDisplay("");

	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);
	w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);

	quit =	XCreateSimpleWindow(dpy, w, 10, 3, 30, 12, BORDER, black, white);
	reset=	XCreateSimpleWindow(dpy, w, 50, 3, 35, 12, BORDER, black, white);
	draw =	XCreateSimpleWindow(dpy, w, 95, 3, 30, 12, BORDER, black, white);
	msg =	XCreateSimpleWindow(dpy, root, 100+300, 100+350, 200, 100, BORDER, black, white);
	ok =		XCreateSimpleWindow(dpy, msg, 150, 70, 40, 20, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);
	XSelectInput(dpy,w,ButtonPressMask);
	XSelectInput(dpy, quit,	ButtonPressMask);
	XSelectInput(dpy, reset, ButtonPressMask);
	XSelectInput(dpy, draw, ButtonPressMask);
	XSelectInput(dpy, ok, ButtonPressMask);
	XSelectInput(dpy, msg, ButtonPressMask);

	XMapWindow(dpy, w);
	XMapSubwindows(dpy, w);
	XMapSubwindows(dpy, msg);

    XSetForeground(dpy,gc,black);

 	while(1){
		XNextEvent(dpy,&e);
		switch(e.type){
		   	case ButtonPress:
		   		if(crd[count-1].x==e.xbutton.x && crd[count-1].y==e.xbutton.y) break;
				if(e.xany.window==quit) return 0;
				if(e.xany.window==reset){
					//リセット動作
					count = 0;
					XClearWindow(dpy, w);
				}
				else if(count > 255){ //257個以上点は打てない
					count = 0;
				}
				else if(e.xany.window==draw){
					if(count < 2){ //点が2個未満のものは排除
						XMapWindow(dpy, msg);
					}else{
                    	//係数行列に値を入れる
                    	for(i=0; i<count; i++){
                    	    for(j=0; j<count; j++){
							coe_matrix[i][j] = pow(crd[i].x, count-j-1);
						}
					}

					//ガウス法で関数の係数と定数項を求める
					for(i=0; i<count; i++){
						for(j=i+1; j<count; j++){
							double quo = (double)coe_matrix[j][i] / coe_matrix[i][i];
							for(k=0; k<count; k++){
								coe_matrix[j][k] -= quo * coe_matrix[i][k];
							}
							crd[j].y -= quo * crd[i].y;
						}
					}
					for(i=count-1; i>-1; i--){
						double dif = 0.0;
						for(j=count-1; j>i; j--){
							dif += sol_matrix[j] * coe_matrix[i][j];
						}
						sol_matrix[i] = (crd[i].y - dif) / coe_matrix[j][i];
					}

						//関数のグラフを表示する
						for(l=0; l<800; l+=0.01){
							y_value = 0;
						for(i=0; i<count; i++)
						y_value += sol_matrix[i] * pow(l, count-i-1);
							if(0 < y_value && y_value < 800)
								XDrawPoint(dpy, w, gc, l, y_value);
						}
						count = 0;
						break;
					}
				}
				else if(e.xany.window==ok){
					XUnmapWindow(dpy, msg);
					XDrawLine(dpy, w, gc, 0, 400, 800, 400);
					XDrawLine(dpy, w, gc, 400, 0, 400, 800);
				}
				else{
					XDrawString(dpy, quit, gc, 4, 10,"Exit", 4);
					XDrawString(dpy, reset, gc, 4, 10, "Reset", 5);
					XDrawString(dpy, draw, gc, 4, 10, "Draw", 4);
					XDrawString(dpy, msg, gc, 80, 15, "Error", 5);
					XDrawString(dpy, msg, gc, 15, 50, "Please input over one point.", 28);
					XDrawString(dpy, ok, gc, 15, 15, "OK", 2);


					//x軸とy軸を表示
					XDrawLine(dpy, w, gc, 0, 400, 800, 400);
					XDrawLine(dpy, w, gc, 400, 0, 400, 800);

					//xとyの構造体に値を入れる
					XDrawPoint(dpy,w,gc,e.xbutton.x,e.xbutton.y);
					crd[count].x = e.xbutton.x;
					crd[count].y = e.xbutton.y;
					count++;
				}
		   	break;
		}
	}
}
