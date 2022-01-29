/*
  XWindowディスプレイ用
*/
#pragma once
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "Parameters.h"

void DisplayInit(int argc, char **argv)
{
  //ウインドウ初期化

  display = XOpenDisplay(NULL); //XWindowサーバに接続
  root = DefaultRootWindow(display);
  screen = DefaultScreen(display);
  white = WhitePixel(display, screen);    //白のピクセル
  black = BlackPixel(display, screen);    //黒のピクセル
  colormap = DefaultColormap(display, 0); //カラーマップ取得
  /*ピクセルを取得*/
  for (int i = 0; i < 3; i++)
  {
    XAllocNamedColor(display, colormap, color_name[i], &c1, &c0);
    color_number[i] = c1.pixel;
  }

  /*ウインドウを作成*/
  window = XCreateSimpleWindow(display,
                               DefaultRootWindow(display),
                               0, 0, WindowWidth, WindowHeight,
                               0, 0, white);
  /*サブウインドウを作成*/
  quit = XCreateSimpleWindow(display, window, 10, 3, 40, 12, window_Border, black, white);

  /*ウインドウのプロパティ指定*/
  XSetStandardProperties(display, window, title, icon_title,
                         None, argv, argc, NULL);
  gc = XCreateGC(display, window, 0, 0);

  /*イベントマスクを指定*/
  XSelectInput(display, window, ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask);
  XSelectInput(display, quit, ButtonPressMask);
}

void OpenDisplay()
{
  //ウインドウ表示
  XMapRaised(display, window);
  XMapSubwindows(display, window);
}
