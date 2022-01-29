/*
  ブロック管理
*/
#pragma once

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <stdlib.h>
#include <time.h> //乱数初期化用
#include "Ball.h"
#include "Slider.h"
#include "Physics.h"

/*ユーザ調整はここから*/
#define BLOCKQ_SIDE 8       //横のブロック数
#define BLOCKQ_VERTICAL 8   //縦のブロック数
const int BlockHeight = 15; //ブロックの高さ
/*ユーザ調整ここまで*/

/*プロトタイプ宣言*/
Vec2 BallDrawPoint;
float BallAngle;

int BlockWidth;   //ブロックの横幅
int BlockCounter; //残りブロック数

/*ブロックマップ*/
int BlockMap[BLOCKQ_VERTICAL][BLOCKQ_SIDE];
//1:ブロックがある,0:ブロックなし

void GenerateMap()
{
  //ブロックマップ作成
  srand((unsigned int)time(NULL)); //乱数初期化
  int k = 1;
  int Random = rand();
  int flag = 0; //ブロックが0になるのを防止
  while (!flag)
  {
    for (int i = 0; i < BLOCKQ_VERTICAL; i++)
    {
      for (int j = 0; j < BLOCKQ_SIDE; j++)
      {
        BlockMap[i][j] = Random & k;

        if (k < 0xffff)
        {
          k = k << 1;
        }
        else
        {
          Random = rand();
          k = 1;
        }
        if (Random & k)
          flag = 1;
      }
      //printf("\n");
    }
  }
}

void BlocksInit()
{
  GenerateMap();
  BlockWidth = WindowWidth / BLOCKQ_SIDE;
}

int isTouchedBlock(Vec2 Block, Vec2 Ball, float angle)
{
  if ((Block.x - BallRadius <= Ball.x && Block.x + BlockWidth + BallRadius >= Ball.x) &&
      (Block.y - BallRadius <= Ball.y && Block.y + BlockHeight + BallRadius >= Ball.y))
    return 1;
  return 0;
}

int UpdateBlocks()
{
  int i, j;
  BlockCounter = 0;
  for (i = 0; i < BLOCKQ_VERTICAL; i++)
  {
    for (j = 0; j < BLOCKQ_SIDE; j++)
    {
      if (BlockMap[i][j])
      {
        BlockCounter++;
        Vec2 BlockPoint = {BlockWidth * j, BlockHeight * i};
        if (isTouchedBlock(BlockPoint, BallDrawPoint, BallAngle))
        {
          //ブロックと触れたら
          BallAngle = 2 * M_PI - BallAngle;
          BlockMap[i][j] = 0;
          break;
        }
      }
    }
  }
  return BlockCounter;
}

void DrawBlocks(Display *dpy, Window w, GC gc)
{
  int i, j;
  //現在の描画をクリア
  XSetForeground(display, gc, white);
  XFillRectangle(dpy, w, gc,
                 0,
                 0,
                 WindowWidth,
                 BlockHeight * BLOCKQ_VERTICAL + 1);
  for (i = 0; i < BLOCKQ_VERTICAL; i++)
  {
    for (j = 0; j < BLOCKQ_SIDE; j++)
    {
      if (BlockMap[i][j])
      {
        XSetForeground(display, gc, color_number[(i + j) % 4]);
        XFillRectangle(dpy, w, gc,
                       BlockWidth * j,
                       BlockHeight * i,
                       BlockWidth,
                       BlockHeight);
        XSetForeground(display, gc, black);
        XDrawRectangle(dpy, w, gc,
                       BlockWidth * j,
                       BlockHeight * i,
                       BlockWidth,
                       BlockHeight);
      }
    }
  }
}