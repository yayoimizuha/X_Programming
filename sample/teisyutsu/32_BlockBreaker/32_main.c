/*
  Block Braker
  Coded by Koichi Hirachi
  Last Update:2020/02/12
*/

#include <stdio.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <time.h> //乱数初期化用
/*XWindow用*/
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "Parameters.h" //グローバル変数定義
#include "EventManeger.h"
#include "Display.h" //ディスプレイ管理
#include "Physics.h"
#include "Ball.h"
#include "Blocks.h"
#include "Slider.h"

int main(int argc, char **argv)
{
  DisplayInit(argc, argv);
  OpenDisplay();
  ObjectInit();
  while (1)
  {
    /*Exitボタンの文字表示*/
    XSetForeground(display, gc, black);
    XSetFont(display, gc, XLoadFont( display, "9x15bold" ));
    XDrawString(display, quit, gc, 4, 10, "Exit", 4);

    XSetFont(display, gc, XLoadFont(display, FONT1));
    if (XEventsQueued(display, QueuedAfterReading))
    {
      /*イベントがある場合*/
      XNextEvent(display, &event); //イベントを取り込む
      switch (event.type)
      {
      case ButtonPress:
        if (event.xany.window == quit)
          return 0;
        break;
      case KeyPress:
        switch (event.xkey.keycode)
        {
        case 38: /*aが押されたとき*/
        case 113: /*左矢印が押された*/
          LeftKeyPressed();
          break;
        case 40: /*dが押されたとき*/
        case 114:/*右矢印が押された*/
          RightKeyPressed();
          break;
        case 65: /*Spaceが押された*/
          StartGame();
          break;
        }
        break;
      case KeyRelease:
        switch (event.xkey.keycode)
        {
        case 38:  /*aが離されたとき*/
        case 40:  /*dが離されたとき*/
        case 113: /*左矢印が離されたとき*/
        case 114: /*右矢印が離されたとき*/
          KeyReleased();
          break;
        }
        break;
      }
    }
    else
    {
      /*イベントがない場合*/
      Update();
      usleep(1000);
    }
  }
}
