#include <stdio.h>
#include<math.h>
#include <string.h>
#include<unistd.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>
#include <X11/keysym.h>

#define BORDER 2

typedef struct{//ベクター型変数の定義
		int x;
		int y;
	}Vector2;

	Display *dpy;
	Window w,quit,r,g,b,y,wh,bl;
	Window root;
	int  screen;
	unsigned long  black, white;
	GC     gc;
	char here[]="I'm here!";
	char moji[256];

	XFontSet fs;//日本語フォントを選ぶ
     XEvent e; //イベント構造体の定義
     Colormap cmap;//カラーマップ構造体
     KeySym key; //キーシンボルが入る場所を宣言

	Window W(Vector2 point,Vector2 size){
	return XCreateSimpleWindow(dpy, root, point.x, point.y, size.x, size.y, 2, black, white);
}

Window subW(Window w,Vector2 point,Vector2 size){
	return XCreateSimpleWindow(dpy, w, point.x, point.y, size.x, size.y, 2, black, white);
}

void setplace(int *ttx, int *tty, int tx, int ty){
	*ttx=tx;
	*tty=ty;
}

void setup(Display *dpy,Window w){
       XSelectInput(dpy,w,ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask | EnterWindowMask | LeaveWindowMask | PointerMotionMask | PointerMotionHintMask | Button1MotionMask | Button2MotionMask | Button3MotionMask | Button4MotionMask | Button5MotionMask | ButtonMotionMask | KeymapStateMask | ExposureMask | VisibilityChangeMask | StructureNotifyMask | ResizeRedirectMask | SubstructureNotifyMask | SubstructureRedirectMask | FocusChangeMask | PropertyChangeMask | ColormapChangeMask | OwnerGrabButtonMask);
}
