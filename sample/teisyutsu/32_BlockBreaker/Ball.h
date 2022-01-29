/*
  ボール用
*/
#pragma once
#include <math.h>
#include "Slider.h"
#include "Blocks.h"
#include "EventManeger.h"

/*プロトタイプ宣言ここから*/
int MarginBottom;
int GameStarted;
Vec2 BlockPoint;
/*プロトタイプ宣言ここまで*/

#define BallDiameter 20 //ボール直径
int BallRadius;         //ボール半径

//ボール座標
Vec2 BallPoint = {0, 0};
//過去のボール座標
Vec2 BallPointP;
Vec2 BallDrawPoint;

/*ボール物理演算定数*/
float BallV;
const float BallAngleDefault = M_PI / 3; //ボールの初期反射角
float BallAngle;                         //ボールの反射角
float BallIntitialVerocity = 0.6;        //ボールの初速度
Vec2 BallVerocity = {0, 0};              //ボールの速度

void BallInit()
{
  BallAngle = BallAngleDefault;
  BallRadius = BallDiameter / 2;
  BallPoint.x = 0;
  BallPoint.y = 0;
}

void DrawBall(Display *dpy, Window w, GC gc)
{
  BallDrawPoint.x = WindowWidth / 2 - BallRadius + BallPoint.x;
  BallDrawPoint.y = WindowHeight - (BallPoint.y + BallRadius + MarginBottom + SliderHeight);
  //一つ前のボールを白で塗りつぶす
  XSetForeground(display, gc, white);
  XFillArc(dpy, w, gc,
           WindowWidth / 2 - BallRadius + BallPointP.x,
           WindowHeight - (BallPointP.y + BallRadius + MarginBottom + SliderHeight),
           BallDiameter, BallDiameter,
           0, 360 * 64);
  //ボールの描画
  XSetForeground(display, gc, color_number[0]); //ボールの色を赤にする
  XFillArc(dpy, w, gc,
           WindowWidth / 2 - BallRadius + BallPoint.x,
           WindowHeight - (BallPoint.y + BallRadius + MarginBottom + SliderHeight),
           BallDiameter, BallDiameter,
           0, 360 * 64);

  BallPointP = BallPoint; //過去座標の更新
}

int isTouchedSlider(Vec2 Ball, Vec2 Slider)
{
  /*ボールとスライダの当たり判定*/
  if ((Ball.x >= Slider.x - SliderWidth / 2 - BallRadius && Ball.x <= Slider.x + SliderWidth / 2 + BallRadius) &&
      (Ball.y <= Slider.y + SliderHeight && Ball.y >= Slider.y - SliderHeight / 2 - BallRadius))
    return 1;
  return 0;
}

int UpdateBall(Display *dpy, Window w, GC gc)
{
  /*ボール位置の更新*/
  if (GameStarted)
  {
    BallVerocity.x = BallIntitialVerocity * cos(BallAngle);
    BallVerocity.y = BallIntitialVerocity * sin(BallAngle) - BallPoint.y * 0.000003;
    BallPoint.x += BallVerocity.x;
    BallPoint.y += BallVerocity.y;
    if (BallPoint.y < -MarginBottom)
    {
      /*ボールが下に落ちたら*/
      BallInit();
      GameStarted = 0;
      return 1;
    }
    if (isTouchedSlider(BallPoint, SliderPoint) && BallVerocity.y < 0)
    {
      /*ボールが落下時にスライダに触れたら*/
      BallAngle = 2 * M_PI - BallAngle;
    }
    if (BallPoint.y > WindowHeight - MarginBottom - BallDiameter)
    {
      //ボールが上の壁に触れたら
      BallAngle = 2 * M_PI - BallAngle;
    }
    if (BallPoint.x > WindowWidth / 2 - BallRadius || BallPoint.x < -WindowWidth / 2 + BallRadius)
    {
      //ボールが左右の壁に触れたら
      BallAngle = M_PI - BallAngle;
    }
  }
  else
  {
    BallPoint.x = SliderPoint.x;
    BallPoint.y = 0;
  }
  return 0;
}
