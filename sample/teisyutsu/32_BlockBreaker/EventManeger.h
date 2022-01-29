/*
  ゲーム全体の管理
*/
#pragma once
#include "Parameters.h"
#include "Ball.h"
#include "Slider.h"
#include "Blocks.h"

int GameStarted = 0;
int GameEnded = 0;

int MarginBottom = 50;

#define LifePoint 3 //残機数
int Life;
int Score = 0;     //ゲームスコア
int BlocksSum = 0; //ブロックの数合計
int BlocksBefo = 0;
float Combo = 0.2; //コンボ時の倍数追加
/*
  スコアは，消したブロック*倍数N
  コンボ中は，倍数NにComboづつ足す
*/

void ObjectInit()
{
  Life = LifePoint;
  BlocksInit();
  BallInit();
  SliderInit();
  BlocksSum = UpdateBlocks();
  Score = 0;
  BlocksBefo = 0;
}

void CalaculateScore(int DeleteBlocks, int mode)
{
  //スコアの計算
  static float ComboSum = 1;
  if (mode)
  {
    ComboSum = 1;
  }
  else
  {
    if (DeleteBlocks)
      ComboSum += Combo;
  }
  float UpScore = DeleteBlocks * ComboSum;
  UpScore = UpScore / BlocksSum * 100; //最大ブロック数による補正
  Score += (int)UpScore;
}

void UpdateLife(int Lifes)
{
  char Life[16];
  XSetForeground(display, gc, white);
  XFillRectangle(display, window, gc,
                 WindowWidth - 100,
                 WindowHeight - 30,
                 100,
                 30);
  sprintf(Life, "LIFE:%d", Lifes);
  XSetForeground(display, gc, black);
  XDrawString(display, window, gc, WindowWidth - 100, WindowHeight - 5, Life, strlen(Life));
}

void GameEnd(int status)
{
  GameEnded = 1;
  char Message[255];
  char Message2[255];
  char PlayerScore[255];
  XSetForeground(display, gc, white);
  XFillRectangle(display, window, gc,
                 0, 0,
                 WindowWidth,
                 WindowHeight);
  if (status)
  {
    sprintf(Message, "Game Clear!");
  }
  else
  {
    sprintf(Message, "Game Over!");
  }
  sprintf(Message2, "Press Space to continue");
  sprintf(PlayerScore, "Score:%d", Score);
  XSetForeground(display, gc, black);
  XDrawString(display, window, gc, WindowWidth / 2 - 65, WindowHeight / 2 - 30, Message, strlen(Message));
  XDrawString(display, window, gc, WindowWidth / 2 - 150, WindowHeight - 50, Message2, strlen(Message2));
  XSetForeground(display, gc, color_number[2]);
  XDrawString(display, window, gc, WindowWidth / 2 - 60, WindowHeight / 2 + 40, PlayerScore, strlen(PlayerScore));
}

void UpdateScore(int Scores)
{
  char Score[16];
  XSetForeground(display, gc, white);
  XFillRectangle(display, window, gc,
                 10,
                 WindowHeight - 30,
                 200,
                 30);
  sprintf(Score, "Score:%d", Scores);
  XSetForeground(display, gc, black);
  XDrawString(display, window, gc, 10, WindowHeight - 5, Score, strlen(Score));
}

void Update()
{
  int Blocks = UpdateBlocks();
  if (Blocks == 0)
  {
    //ゲームクリア
    GameEnd(1);
  }
  else if (Life == 0)
  {
    //ゲームオーバ
    GameEnd(0);
  }
  else
  {
    DrawBlocks(display, window, gc);
    static int flag = 0;
    switch (UpdateBall(display, window, gc))
    {
    case 1: //ボールが落ちたら
      Life--;
      break;
    case 2: //ボールがスライダに触れた
      if (!flag)
        flag = 1;
      if (flag)
      {
        if (BlocksBefo)
          CalaculateScore(BlocksBefo - Blocks, 1);
        flag = 0;
      }
      break;
    }
    DrawBall(display, window, gc);
    DrawSlider(display, window, gc);
    UpdateLife(Life);
    UpdateScore(Score);
    if (BlocksBefo)
      CalaculateScore(BlocksBefo - Blocks, 0);
    BlocksBefo = Blocks;
  }
  XFlush(display);
}

void StartGame()
{
  if (GameEnded)
  {
    XClearWindow(display, window);
    GameEnded = 0;
    ObjectInit();
  }
  GameStarted = 1;
}

void RightKeyPressed()
{
  SliderMoveRight();
}

void LeftKeyPressed()
{
  SliderMoveLeft();
}

void KeyReleased()
{
  SliderStop();
}
