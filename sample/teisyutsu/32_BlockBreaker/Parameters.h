/*
   各種グローバル変数定義用ファイル
 */

#pragma once

/*以下XWindows用*/
//ウインドウ設定
#define WindowWidth 400         //ウインドウ横幅
#define WindowHeight 500        //ウインドウ縦幅
const int window_Border = 2;    //枠の幅
char title[] = "Block Breaker"; //タイトル
char icon_title[] = "ICON";     //アイコンタイトル

Display *display; //ディスプレイ
Window window;    //メインウィンドウ
Window quit;      //終了ボタン
Window root;
GC gc;
XEvent event; //Xイベント共用体
int screen;

typedef struct
{
  Display *display;
  Window window;
  GC gc;
} dispInfo;

dispInfo Disp;

char color_name[4][20] = {"red", "green", "blue", "purple"};
unsigned long color_number[4];
Colormap colormap; //カラーマップ構造体
XColor c0, c1;     //カラー構造体
unsigned long white;
unsigned long black;

/*フォント指定*/
char *FONT1 = "-adobe-helvetica-bold-r-normal--24-240-75-75-p-138-iso8859-1";
