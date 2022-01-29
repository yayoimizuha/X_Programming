#pragma once
#include "Parameters.h"
#include "Physics.h"
#include "EventManeger.h"
#include "Ball.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

/*プロトタイプ宣言ここから*/
int MarginBottom;
int BallRadius;
/*プロトタイプ宣言ここまで*/

#define SliderWidth 50  //スライダの横幅
#define SliderHeight 15 //スライダの縦幅

Vec2 SliderPoint;  //スライダ座標
Vec2 SliderPointP; //過去のスライダ座標
float SliderVerocity = 0;
float SliderSpeed = 0.8;

void SliderInit()
{
  SliderPoint.x = 0;
  SliderPoint.y = -BallRadius;
}

void SliderCheck()
{
  /*スライダのはみ出し防止*/
  if (SliderPoint.x > WindowWidth / 2 - SliderWidth / 2)
    SliderPoint.x = WindowWidth / 2 - SliderWidth / 2;
  if (SliderPoint.x < -WindowWidth / 2 + SliderWidth / 2)
    SliderPoint.x = -WindowWidth / 2 + SliderWidth / 2;
}

void SliderMoveRight()
{
  if (SliderPoint.x < WindowWidth / 2 - SliderWidth / 2)
    SliderVerocity = SliderSpeed;
}

void SliderMoveLeft()
{
  if (SliderPoint.x > -WindowWidth / 2 + SliderWidth / 2)
    SliderVerocity = -SliderSpeed;
}

void SliderStop()
{
  SliderVerocity = 0;
}

void DrawSlider(Display *dpy, Window w, GC gc)
{
  SliderCheck();
  SliderPoint.x += SliderVerocity;
  //一つ前のスライダを白で塗りつぶす
  XSetForeground(display, gc, white);
  XFillRectangle(dpy, w, gc,
                 WindowWidth / 2 - SliderWidth / 2 + SliderPointP.x,
                 WindowHeight - (SliderPointP.y + SliderHeight + MarginBottom),
                 SliderWidth,
                 SliderHeight);

  XSetForeground(display, gc, black);
  //スライダの描画
  XFillRectangle(dpy, w, gc,
                 WindowWidth / 2 - SliderWidth / 2 + SliderPoint.x,
                 WindowHeight - (SliderPoint.y + SliderHeight + MarginBottom),
                 SliderWidth,
                 SliderHeight);

  SliderPointP = SliderPoint; //過去座標の更新
}
