/*
  各種物理演算用
*/
#pragma once
typedef struct
{
  float x;
  float y;
} Vec2;

float g = 9.8; //重力加速度:0にすればレーザっぽくなる
float t = 0;   //時間[s]