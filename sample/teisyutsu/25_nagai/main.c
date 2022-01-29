/*プロ技提出用メインプログラム*/
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <X11/Xlib.h>
#include "ball.h"

long getTime();

int main() {
  //Xディスプレイサーバへの接続
  Display *dpy;
  dpy = XOpenDisplay("");
  if(!dpy) {
    printf("Xディスプレイサーバへの接続に失敗しました．\n");
    return 0;
  }

  //定数定義
  const int fps = 60;

  //変数宣言
  Window root, w, simulateArea, configArea;
  Pixmap simulateAreaPixMap;
  XWindowAttributes displayInfo;
  XEvent event;
  GC gc, gc_clr;
  int screen;
  int displayWidth, displayHeight;
  long time2, time1, interval, dt;
  unsigned long black, white;

  //画面の準備
  root = DefaultRootWindow(dpy);
  screen = DefaultScreen(dpy);
  white = WhitePixel(dpy, screen);
  black = BlackPixel(dpy, screen);
  XGetWindowAttributes(dpy, root, &displayInfo);
  displayWidth = displayInfo.width;
  displayHeight = displayInfo.height;
  displayWidth = 2560;
  field field1 = {9.8, displayWidth, displayHeight};


  //1秒あたりのフレーム数を設定
  interval = ((1 * 1000 * 1000) / fps);

  //メインウインドウ定義
  w = XCreateSimpleWindow(dpy, root, 0, 0, displayWidth, displayHeight, 1, black, white);
  XMapWindow(dpy, w);

  //GC定義
  //描画用
  gc = XCreateGC(dpy, w, 0, 0);
  XSetBackground(dpy, gc, white);
  XSetForeground(dpy, gc, black);

  //消去用
  gc_clr = XCreateGC(dpy, w, 0, 0);
  XSetBackground(dpy, gc_clr, white);
  XSetForeground(dpy, gc_clr, white);

  //シミュレーション領域設定
  simulateArea = XCreateSimpleWindow(dpy, w, 0, 0, displayWidth*0.9, displayHeight, 1, black, white);
  XSelectInput(dpy, simulateArea, ButtonPressMask | ButtonReleaseMask | MotionNotify);
  simulateAreaPixMap = XCreatePixmap(dpy, w, displayWidth*0.9, displayHeight, DefaultDepth(dpy, 0));


  XMapSubwindows(dpy, w);

    //ball制御
    ball ball1;
    ball1.position[0] = 100;
    ball1.position[1] = 100;
    ball1.speed[0] = 0;
    ball1.speed[1] = 0;
    ball1.radius = 100;
    ball1.e = 1;

  time1 = getTime();
  while(1) {
    time2 = getTime();
    if(time2 < time1 + interval) {
      continue;
    }
    XFillRectangle(dpy, simulateAreaPixMap, gc_clr, 0, 0, displayWidth, displayHeight);
    dt = time2 - time1;

    moveBall(&ball1, &field1, dt);
    
    XDrawArc(dpy, simulateAreaPixMap, gc, ball1.position[0], ball1.position[1], ball1.radius, ball1.radius, 0, 64*360);
     XCopyArea(dpy, simulateAreaPixMap, simulateArea, gc, 0, 0, (int) displayWidth*0.9, displayHeight, 0, 0);
    
    XFlush(dpy);
    time1 = getTime();
  }
}

//時間計測用関数/μSで出力
long getTime() {
  struct  timespec time;
  unsigned long returnTime;
  clock_gettime(CLOCK_MONOTONIC, &time);
  returnTime = time.tv_nsec / 1000;
  returnTime += (long) time.tv_sec * 1000000;
  return returnTime;
}