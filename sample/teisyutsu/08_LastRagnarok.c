/*
space key で大砲発射
*/

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xlocale.h>
#include <X11/keysym.h>
#include <unistd.h>
#include <math.h>

#define BORDER 2
#define WIDTH 1200
#define HIGHT 700
#define z 50

//やっぱりboolが無いと不便なので
typedef int bool;

bool true = 1;
bool false = 0;

//いちいち情報書くのめんどいので、構造体にして一気に渡す。
struct dispinfo{
  Display *dpy;
  Window w;
  GC gc;
};

//ものの座標を入れる。
struct coodin{
  int x;
  int y;
  int width;
  int hight;
  int Rside;
  int Lside;
  int Uside;
  int Dside;
};

//ものの情報を入れる。
struct status{
  float g;
  float v0;
  float v;
  float t;
  int x;
  int y;
  int size;
  bool busy;
};

//自分で作っといてあれだけど構造体の中身結構多くなっちゃったので関数で作る。
struct status MakeSphere(int x, int y, int size);
struct coodin MakeBox(int x, int y, int width, int hight);
struct coodin MakeCanon(int x, int y, int size);

//度数表記をラジアンに直す
float DegreForRad(float degree);

//球を描画する関数
void ball(struct dispinfo info, float x, float y, int wide);
//Triangle
void Triangle(struct dispinfo info, int x, int y, int scale, bool toFill);
//壁や床を描画する関数
void wall(struct dispinfo info, struct coodin object);
//大砲描画
void canon(struct dispinfo info, int x, int y, int size, int whsize, int whthick, unsigned long white, unsigned long *color);
//部屋を描画
void room(struct dispinfo info, int L, int R, int U, int D, int Zline, unsigned long black, unsigned long *color);
//正弦波を発振する
void sinGraph(struct dispinfo info, int L, int R, int U, int A, int value);
//波紋って英語でRippleなんですね。
void Ripple(struct dispinfo info, int x, int y, int size, unsigned long maincolor, unsigned long backcolor);

//引数内のオブジェクトをジャンプさせる関数
struct status Jump(struct status object, float floorY, float cellingY, float JumpPow);

int startdegree = 90;
int rippletime = 0;
bool startripple;
int rippleX,rippleY;

int main(int argc, char **argv){
  Display *dpy;
  Window w,quit;
  Window root;
  int screen;
  unsigned long black, white;
  GC gc;
  XEvent e;
  KeySym key;

  Colormap cmap;
  XColor c0,c1;
  char color_name[8][20] = {"red","green","cyan","gray19","maroon1","gray100","lightpink"};
  unsigned long c_no[8];

  dpy = XOpenDisplay("");

  root = DefaultRootWindow(dpy);
  screen = DefaultScreen(dpy);
  white = WhitePixel(dpy, screen);
  black = BlackPixel(dpy, screen);
  cmap = DefaultColormap(dpy, 0);/*カラーマップを得る*/
  for(int i = 0;i < 8;i++){
    XAllocNamedColor(dpy,cmap,color_name[i],&c1,&c0);/* c1.pixel */
    c_no[i] = c1.pixel;
  }

//  w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, black, white);//メインウィンドウ作成
  w = XCreateSimpleWindow(dpy, root, 100, 100, WIDTH, HIGHT, BORDER, c_no[1], black);//メインウィンドウ作成

  quit = XCreateSimpleWindow(dpy, w, 10, 3, 100, 35, BORDER, c_no[1], black);//quitボタン用のウィンドウ作成

  gc = XCreateGC(dpy, w, 0, NULL);//グラフィック設定

  XSelectInput(dpy, w, ButtonPressMask | KeyPressMask | KeyReleaseMask);//メイン
  XSelectInput(dpy, quit, ButtonPressMask);//サブ

  XMapWindow(dpy, w);
  XMapSubwindows(dpy, w);

  XSetForeground(dpy, gc, black);

  /*----------ここまで画面やらウィンドウやらの設定----------*/

  int disborder = 100;
  int line = 1;

  struct dispinfo info = {dpy, w, gc};
  //            sphere = {    a,  v0,   v, t,   x,  y,size};
  //struct status sphere = {0.98, 0, 0, 0, 450, 500, 15, false};

  //struct coodin floor = {50,600,900,50};

  struct coodin floor = MakeBox(disborder,HIGHT-line-disborder,WIDTH-disborder*2,line);
  struct coodin rightwall = MakeBox(WIDTH-line-disborder,disborder,line,HIGHT-line-disborder*2);
  struct coodin leftwall = MakeBox(disborder,disborder,line,HIGHT-line-disborder*2);
  struct coodin celling = MakeBox(floor.x,rightwall.Uside+line,floor.width,line);

  struct status sphere = MakeSphere(-50, -50, 30);
  const int sphstartX = disborder + 50 - z, sphstartY = floor.Uside - sphere.size + z;

  struct coodin canonfire = MakeCanon((sphstartX-50)*3.5,sphstartY,0);

  enum dir{right,left};
  int way = right;

  int BurstPower = 50;
  float seta = DegreForRad(45);

  int debugcount = 0;

  int Zline = 20;
  startripple = false;
  float vmin,vmax;
  vmin = 620;
  vmax = 0;

  //XAutoRepeatOff(dpy);

  while(1){//----------------while start-----------------------------

    XClearWindow(dpy,w);

    if(XEventsQueued(dpy, QueuedAfterReading)){
      XNextEvent(dpy, &e);
      switch(e.type){
        case ButtonPress:
          if(e.xany.window == quit){
            return 0;
          }
          break;
        case KeyPress:
          printf("keycode = %d\n",e.xkey.keycode);
          if(e.xkey.keycode == 65){//space key
            //printf("space key is pressed.\n");
            sphere.x = sphstartX;
            sphere.y = sphstartY;
            way = right;
          }else if(e.xkey.keycode == 9){//escape key
            return 0;
          }
          break;
        case KeyRelease:
          if(e.xkey.keycode == 65){
            //printf("%d\n",debugcount);

            sphere = Jump(sphere,floor.Uside,celling.Dside,BurstPower*sin(seta));
          }
          break;

      }
    }/*アニメーションの描画はこの中に書く*/
      /*---描画関係---*/
      //Exitボタンの描画
      XSetForeground(dpy,gc,c_no[1]);
      XDrawString(dpy, quit, gc, 4, 10, "Exit", 4);
      //printf("%d %f\n", sphere.busy, sphere.v);
      wall(info,rightwall);//右側に壁を追加
      wall(info,leftwall);//左側に壁を追加
      wall(info,celling);//天井を追加
      wall(info,floor);//床を設置
      room(info, celling.Lside, celling.Rside, celling.Uside, floor.Dside, Zline, black, c_no);
      if(startripple == true){
        Ripple(info, rippleX, rippleY, 80, c_no[5], black);
      }
      XSetForeground(dpy, gc, c_no[2]);
      XFillRectangle(info.dpy, info.w, info.gc, 540, 540, ((sphere.v-vmin)/(vmax-vmin))*520, 10);
      XSetForeground(dpy, gc, c_no[4]);
      ball(info,sphere.x,sphere.y,sphere.size);

      /*---計算類---*/
      //sphere.y -= sphere.dy;
      if(sphere.busy == true){
        sphere = Jump(sphere,floor.Uside,celling.Dside,BurstPower*sin(seta));
        if(way == right){
          sphere.x += BurstPower*cos(seta);
        }else if(way == left){
          sphere.x -= BurstPower*cos(seta);
        }
      }

      if(sphere.v > vmax){
        vmax = sphere.v;
      }else if(sphere.v < vmin){
        vmin = sphere.v;
      }

      if(sphere.x + sphere.size >= rightwall.Lside + z){
        sphere.x = rightwall.Lside + z - sphere.size;
        way = left;
        startripple = true;
        rippleX = sphere.x;
        rippleY = sphere.y;
      }else if(sphere.x <= leftwall.Rside - z){
        sphere.x = leftwall.Rside - z;
        way = right;
        startripple = true;
        rippleX = sphere.x;
        rippleY = sphere.y;
      }

      XSetForeground(dpy, gc, c_no[1]);
      canon(info, 48, 470, 180, 130, 13, c_no[6], c_no);

      usleep(16666);
      XFlush(dpy);

  }
}

//---------main end------------

void ball(struct dispinfo info, float x, float y, int wide){
  XDrawArc(info.dpy, info.w, info.gc, (int)x, (int)y, wide, wide, 0, 64*360);
  XFillArc(info.dpy, info.w, info.gc, (int)x+3, (int)y+3, wide-6, wide-6, 0, 64*360);
}

void wall(struct dispinfo info, struct coodin object){
  XFillRectangle(info.dpy, info.w, info.gc, object.x, object.y, object.width, object.hight);
}

void Triangle(struct dispinfo info, int x, int y, int scale, bool toFill){
  XPoint points[4] = {{x,y},{scale,scale * -1.73},{scale,scale * 1.73},{scale * -2,0}};
  int npoints = 4;

  if(toFill == true){
    XFillPolygon(info.dpy, info.w, info.gc, points, npoints, Complex, CoordModePrevious);
  }else if(toFill == false){
    XDrawLines(info.dpy, info.w, info.gc, points, npoints, CoordModePrevious);
  }
}

void canon(struct dispinfo info, int x, int y, int size, int whsize, int whthick, unsigned long white, unsigned long *color){
  XSetForeground(info.dpy, info.gc, color[4]);
  XFillArc(info.dpy, info.w, info.gc, x, y, size, size, 0, 64*360);
  XSetForeground(info.dpy, info.gc, white);
  XFillArc(info.dpy, info.w, info.gc, x, y+size-whsize, whsize, whsize, 0, 64*360);
  XDrawArc(info.dpy, info.w, info.gc, x, y+size-whsize, whsize, whsize, 0, 64*360);
  XSetForeground(info.dpy, info.gc, color[4]);
  XFillArc(info.dpy, info.w, info.gc, x+whthick, y+size-whsize+whthick, whsize-whthick*2, whsize-whthick*2, 0, 64*360);
  XSetForeground(info.dpy, info.gc, white);
  XPoint points1[5] = {{x+(whsize-whthick)/2.0,y+size-whsize+whthick/2.0},{whthick,0},{0,whsize-whthick},{-whthick,0},{0,whthick-whsize}};
  XPoint points2[5] = {{x+((whsize-whthick)/2.0)-(whsize*tan(DegreForRad(60))/4.0)+(whthick/2.0),y+size-whsize+((whsize-whthick)/4.0)+(whthick*3/4.0)},{whthick*cos(DegreForRad(60)),-1*whthick*sin(DegreForRad(60))},{(whsize-whthick)*cos(DegreForRad(30)),(whsize-whthick)*sin(DegreForRad(30))},{-1*whthick*cos(DegreForRad(60)),whthick*sin(DegreForRad(60))},{(whthick-whsize)*cos(DegreForRad(30)),(whthick-whsize)*sin(DegreForRad(30))}};
  XPoint points3[5] = {{x+((whsize+whthick)/2.0)+(whsize*tan(DegreForRad(60))/4.0)-(whthick/2.0),y+size-whsize+((whsize-whthick)/4.0)+(whthick*3/4.0)},{-1*whthick*cos(DegreForRad(60)),-1*whthick*sin(DegreForRad(60))},{-1*(whsize-whthick)*cos(DegreForRad(30)),(whsize-whthick)*sin(DegreForRad(30))},{whthick*cos(DegreForRad(60)),whthick*sin(DegreForRad(60))},{-1*(whthick-whsize)*cos(DegreForRad(30)),(whthick-whsize)*sin(DegreForRad(30))}};

  int muzzlethick = 40;int muzzlehole = size;
  XPoint points4[4] = {{x+(size/2.0)+5,y-13},{muzzlethick*cos(DegreForRad(30)),-1*muzzlethick*sin(DegreForRad(30))},{muzzlehole*sin(DegreForRad(30)),muzzlehole*cos(DegreForRad(30))},{-1*muzzlethick*cos(DegreForRad(30)),muzzlethick*sin(DegreForRad(30))}};

  int npoints = 5;
  XFillPolygon(info.dpy, info.w, info.gc, points1, npoints, Complex, CoordModePrevious);
  XFillPolygon(info.dpy, info.w, info.gc, points2, npoints, Complex, CoordModePrevious);
  XFillPolygon(info.dpy, info.w, info.gc, points3, npoints, Complex, CoordModePrevious);
  XFillPolygon(info.dpy, info.w, info.gc, points4, 4, Complex, CoordModePrevious);
}

void room(struct dispinfo info, int L, int R, int U, int D, int Zline, unsigned long black, unsigned long *color){
  int devideY[6];
  int devideX[6];
  for(int i = 0;i < 6;i++){
    devideX[i] = (WIDTH / 5.0) * i;
    devideY[i] = (HIGHT / 5.0) * i;
  }
  int centerX = WIDTH/2.0;
  int centerY = HIGHT/2.0;

  for(int i = 1;i < 5;i++){
    XDrawLine(info.dpy, info.w, info.gc, centerX, centerY, devideX[i], 0);
    XDrawLine(info.dpy, info.w, info.gc, centerX, centerY, devideX[i], HIGHT);

    XDrawLine(info.dpy, info.w, info.gc, centerX, centerY, 0, devideY[i]);
    XDrawLine(info.dpy, info.w, info.gc, centerX, centerY, WIDTH, devideY[i]);
  }
  XDrawLine(info.dpy, info.w, info.gc, 0, 0, L, U);
  XDrawLine(info.dpy, info.w, info.gc, 0, HIGHT, L, D);
  XDrawLine(info.dpy, info.w, info.gc, WIDTH, 0, R, U);
  XDrawLine(info.dpy, info.w, info.gc, WIDTH, HIGHT, R, D);
  for(int i = 1; L - Zline*i > 0; i++){
    XDrawRectangle(info.dpy, info.w, info.gc, L - Zline*i, U - Zline*i, R-L + Zline * i * 2, D-U + Zline * i * 2);
  }
  XSetForeground(info.dpy, info.gc, black);
  XFillRectangle(info.dpy, info.w, info.gc, L, U, R-L, D-U);

  XSetForeground(info.dpy, info.gc, color[1]);
  XDrawRectangle(info.dpy, info.w, info.gc, L, U, R-L, D-U);
  //ここまでが部屋の形を作る
  L = L+100;
  R = R-600;
  U = U+150;
  D = D-350;

  XSetForeground(info.dpy, info.gc, color[2]);
  sinGraph(info, L, R, U, 50, 15);
  XDrawRectangle(info.dpy, info.w, info.gc, L, U-50, R-L, 2*50+100);

  int A = 60;
  R = R + 100;
  U = U + 20;
  XDrawArc(info.dpy, info.w, info.gc, R + 100 + A*cos(5 * DegreForRad(startdegree)), U + 50 + A*sin(5 * DegreForRad(startdegree)), 30, 30, 0, 64*360);
  XDrawArc(info.dpy, info.w, info.gc, R + 100 + A*cos(6 * DegreForRad(startdegree)), U + 50 + A*sin(6 * DegreForRad(startdegree)), 20, 20, 0, 64*360);

  R = R + 80;
  U = U - 40;
  XDrawArc(info.dpy, info.w, info.gc, R, U, 150, 150, 0, 64*360);
  R = R + 60;
  U = U + 55;
  XDrawArc(info.dpy, info.w, info.gc, R, U, 250, 250, 0, 64*360);
  R = R + 40;
  U = U - 50;
  XDrawArc(info.dpy, info.w, info.gc, R, U, 200, 200, 0, 64*360);
  XFillArc(info.dpy, info.w, info.gc, R+2, U+2, 180, 180, 0, 64*360);

  A = 50;
  XSetForeground(info.dpy, info.gc, color[3]);
  XFillArc(info.dpy, info.w, info.gc, R+60 + A*sin(9 * DegreForRad(startdegree)), U+60 + A*sin(9 * DegreForRad(startdegree)), 30, 30, 0, 64*360);
  XFillArc(info.dpy, info.w, info.gc, R+80 - A*cos(7 * DegreForRad(startdegree)), U+100 + A*cos(7 * DegreForRad(startdegree)), 30, 30, 0, 64*360);
  XSetForeground(info.dpy, info.gc, color[2]);

  Triangle(info, 500, 500, 50, false);
  Triangle(info, 550, 350, 50, false);
  Triangle(info, 1000, 200, 30, true);

  sinGraph(info, 200, 350, 450, 15, 30);
  sinGraph(info, 200, 350, 450, 17, 25);
  XDrawRectangle(info.dpy, info.w, info.gc, 200, 435, 150, 64);

  XDrawRectangle(info.dpy, info.w, info.gc, 540, 370, 520, 160);
  XDrawRectangle(info.dpy, info.w, info.gc, 540, 540, 520, 10);

  XSetForeground(info.dpy, info.gc, color[1]);
  startdegree++;
}

void sinGraph(struct dispinfo info, int L, int R, int U, int A, int value){
  for(int i = L; i <= R;i++){
    XDrawPoint(info.dpy, info.w, info.gc, i, A*sin(value*(i+startdegree/360.0)*2*3.14)+U+A);
  }
}

void Ripple(struct dispinfo info, int Ox, int Oy, int size, unsigned long maincolor, unsigned long backcolor){
  int x,y;
  x = Ox - size/2.0;
  y = Oy - size/2.0;
  int aftX,aftY;
  aftX = (rippletime - (size/4.0))*2;
  aftY = (rippletime - (size/4.0))*2;
  XSetForeground(info.dpy, info.gc, maincolor);
  if(rippletime < size/2.0){
    XFillArc(info.dpy, info.w, info.gc, Ox-rippletime, Oy-rippletime, rippletime*2, rippletime*2, 0, 64*360);
  }else{
    XFillArc(info.dpy, info.w, info.gc, x, y, size, size, 0, 64*360);
  }
  XSetForeground(info.dpy, info.gc, backcolor);
  if(rippletime > size/4.0){
    XFillArc(info.dpy, info.w, info.gc, Ox-aftX, Oy-aftY, aftX*2, aftY*2, 0, 64*360);
  }
  if(rippletime >= size/2.0){
    rippletime = 0;
    startripple = false;
  }
  rippletime+=2;
}

struct status Jump(struct status object, float floorY, float cellingY, float JumpPow){

  if(object.busy == false){
    object.v = JumpPow * -1;
    object.busy = true;
  }

  object.v += object.g;//1ループにつき、vがa増える。
  object.y += object.v;//変位 = (a*t^2)/2

  if(object.y + object.size > floorY + z){
    object.y = floorY + z - object.size;
    object.v *= -0.6;
    startripple = true;
    rippleX = object.x;
    rippleY = object.y;
  }
  if(object.y < cellingY - z){
    object.y = cellingY - z;
    object.v *= -0.6;
    startripple = true;
    rippleX = object.x;
    rippleY = object.y;
  }
  //printf("sphere's Y : %d, floor's Y : %f\n", object.y + object.size, floorY);

  //最頂点で速度が一番低くなるはずなので速度が小さい時の高さが床とほぼ同じならジャンプ終わり
  if((object.y + object.size + 3 >= floorY + z) && (object.v <= 1) && (object.v >= -1)){
    object.y = floorY + z - object.size;
    object.busy = false;
  }
  return object;
}

float DegreForRad(float degree){
  float rad;
  rad = (degree/360.0)*2*3.14159;
  return rad;
}

struct status MakeSphere(int x, int y, int size){
  struct status sphere;
  sphere.g = 0.98;
  sphere.v0 = 0;
  sphere.v = 0;
  sphere.t = 0;
  sphere.x = x;
  sphere.y = y;
  sphere.size = size;
  sphere.busy = false;

  return sphere;
}

struct coodin MakeBox(int x, int y, int width, int hight){
  struct coodin box;
  box.x = x;
  box.y = y;
  box.width = width;
  box.hight = hight;
  box.Rside = x + width;
  box.Lside = x;
  box.Uside = y;
  box.Dside = y + hight;

  return box;
}

struct coodin MakeCanon(int x, int y, int size){
  struct coodin canons;
  canons.x = x;
  canons.y = y;
  canons.width = 0;
  canons.hight = 0;
  canons.Rside = 0;
  canons.Lside = 0;
  canons.Uside = 0;
  canons.Dside = 0;

  return canons;
}
