/*    35_sort.c    */

// ライブラリインクルード
#include <X11/Xlib.h>
#include <X11/Xlocale.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// 定数
#define DATA_MAX      50                               // データ要素数
#define COLOR_MAX    753                               // カラー種数
#define WIN_WIDTH    800                               // ウインドウサイズ(横)
#define WIN_HEIGHT   (int)(9.0 / 16.0 * WIN_WIDTH)     // ウインドウサイズ(縦)
#define GRAPH_HEIGHT (int)(WIN_HEIGHT - 114)           // グラフサイズ(縦)
#define GRAPH_WIDTH  (int)(16.0 / 9.0 * GRAPH_HEIGHT)  // グラフサイズ(横)

// プロトタイプ宣言
void SelectionSort(Display *dpy, Window *win, XFontSet fs, GC gc, unsigned long *c, int *data, int *order, int size, int *count, int delay);
void BubbleSort(Display *dpy, Window *win, XFontSet fs, GC gc, unsigned long *c, int *data, int *order, int size, int *count, int delay);
void QuickSort(Display *dpy, Window *win, XFontSet fs, GC gc, unsigned long *c, int *data, int *order, int size, int *count, int delay);
void DrawGraph(Display *dpy, Window *win, XFontSet fs, GC gc, unsigned long *c, int *data, int *order, int size, int *count, int delay, int flag);
void DrawStringFormat(Display *dpy, Window win, XFontSet fs, GC gc, unsigned long *c, char *str, int len, int flag_x, int flag_y);
void DrawNumFormat(Display *dpy, Window win, XFontSet fs, GC gc, unsigned long *c, int num, int flag_x, int flag_y);
void NumConsole(Display *dpy, Window win, XFontSet fs, GC gc, unsigned long *c, char *letter, char *str, int *pos, int pos_lim, int space_flag, int flag_x, int flag_y);
void DrawCursor(Display *dpy, Window win, XFontSet fs, GC gc, unsigned long *c, char *str, int len, int pos, int flag_x, int flag_y);
void ColorSet(Display *dpy, char color_name[][20], unsigned long *c);
void GetWinSize(Display *dpy, Window win, int *w, int *h);
void swap(int *a, int *b);
int StrToArray(char *str, int len, int *data, int size);
int MaxArray(int *data, int size);

// メイン関数
int main(int argc, char **argv) {
    // 変数
    int data[DATA_MAX], order[DATA_MAX];
    int delay = 0, count, size;
    int pos0 = 0, pos1 = 0, pos0_lim, pos1_lim;
    int menu_flag = 0;
    int i, w, h;

    // テキスト設定
    char sort_type_text[3][10] = {"Selection", "Bubble", "Quick"};
    char graph_text[5][10] = {"", "Sort Type", "Delay[ms]", "Data Size", "Exchange"};
    char exit_button_text[5] = {"Exit"};
    char start_button_text[6] = {"Start"};
    char here[9], string[9], data_text[512], delay_text[64];

    for (i = 0; i < 512; i++)
        data_text[i] = '\0';
    for (i = 0; i < 64; i++)
        delay_text[i] = '\0';

    Display *dpy;
    Window root, win;
    Window ExitButton, StartButton, TextBox, MenuBar, MenuWin, Menu[3], GraphArea[5];
    Window SortType, temp;
    GC gc;
    XEvent e;
    KeySym key;
    int screen;
    unsigned long black, white;

    dpy = XOpenDisplay("");
    root = DefaultRootWindow(dpy);
    screen = DefaultScreen(dpy);
    white = WhitePixel(dpy, screen);
    black = BlackPixel(dpy, screen);

    w = WIN_WIDTH - (GRAPH_WIDTH + 30);
    h = (GRAPH_HEIGHT - 3 * 10) / 4;

    // ウインドウ生成
    win = XCreateSimpleWindow(dpy, root, 10, 10, WIN_WIDTH, WIN_HEIGHT, 1, black, white);
    ExitButton = XCreateSimpleWindow(dpy, win, 10, 10, 48, 32, 1, black, white);
    StartButton = XCreateSimpleWindow(dpy, win, (WIN_WIDTH - 58), 52, 48, 32, 1, black, white);
    TextBox = XCreateSimpleWindow(dpy, win, 10, 52, (WIN_WIDTH - 78), 32, 1, black, white);
    GraphArea[0] = XCreateSimpleWindow(dpy, win, 10, 94, GRAPH_WIDTH, GRAPH_HEIGHT, 1, black, white);
    GraphArea[1] = XCreateSimpleWindow(dpy, win, (GRAPH_WIDTH + 20), (94 + 0 * (h + 10)), w, h, 1, black, white);
    GraphArea[2] = XCreateSimpleWindow(dpy, win, (GRAPH_WIDTH + 20), (94 + 1 * (h + 10)), w, h, 1, black, white);
    GraphArea[3] = XCreateSimpleWindow(dpy, win, (GRAPH_WIDTH + 20), (94 + 2 * (h + 10)), w, h, 1, black, white);
    GraphArea[4] = XCreateSimpleWindow(dpy, win, (GRAPH_WIDTH + 20), (94 + 3 * (h + 10)), w, (GRAPH_HEIGHT - 3 * (h + 10)), 1, black, white);
    MenuBar = XCreateSimpleWindow(dpy, GraphArea[1], (8 * strlen(graph_text[1]) + 12), 8, 20, 18, 1, white, white);

    // イベントマスク
    XSelectInput(dpy, win, (ButtonPressMask | KeyPressMask));
    XSelectInput(dpy, ExitButton, ButtonPressMask);
    XSelectInput(dpy, StartButton, ButtonPressMask);
    XSelectInput(dpy, TextBox, (ButtonPressMask | KeyPressMask));
    XSelectInput(dpy, GraphArea[1], ButtonPressMask);
    XSelectInput(dpy, GraphArea[2], (ButtonPressMask | KeyPressMask));
    XSelectInput(dpy, MenuBar, ButtonPressMask);

    // ウインドウ表示
    XMapWindow(dpy, win);
    XMapSubwindows(dpy, win);
    XMapSubwindows(dpy, GraphArea[1]);
    XMapSubwindows(dpy, GraphArea[2]);

    gc = XCreateGC(dpy, win, 0, NULL);

    // テキストボックスの文字数制限
    GetWinSize(dpy, TextBox, &w, &h);
    pos0_lim = (w - 16) / 8;
    GetWinSize(dpy, GraphArea[2], &w, &h);
    pos1_lim = (w - 16) / 8;

    // フォント設定(日)
    char **miss, *def;
    int n_miss;
    XFontSet fs;
    setlocale(LC_ALL, "");
    fs = XCreateFontSet(dpy, "-*-*-medium-r-normal-*-16-*", &miss, &n_miss, &def);

    // カラー設定
    char color_name[DATA_MAX][20];
    unsigned long c[DATA_MAX];
    ColorSet(dpy, color_name, c);

    //  ループ
    while (1) {
        if (XEventsQueued(dpy, QueuedAfterReading)) {
            XNextEvent(dpy, &e);
            switch (e.type) {
                case ButtonPress:
                    // 終了
                    if (e.xany.window == ExitButton) {
                        return 0;
                    }
                    // ソート開始
                    else if (e.xany.window == StartButton) {
                        size = StrToArray(data_text, strlen(data_text), data, DATA_MAX);
                        StrToArray(delay_text, strlen(delay_text), &delay, 1);
                        count = 0;
                        for (i = 0; i < DATA_MAX; i++)
                            order[i] = i;

                        if (SortType == Menu[0])
                            SelectionSort(dpy, GraphArea, fs, gc, c, data, order, size, &count, delay);
                        else if (SortType == Menu[1])
                            BubbleSort(dpy, GraphArea, fs, gc, c, data, order, size, &count, delay);
                        else if (SortType == Menu[2])
                            QuickSort(dpy, GraphArea, fs, gc, c, data, order, size, &count, delay);
                    }
                    // テキストボックスの選択
                    else if ((e.xany.window == TextBox) || (e.xany.window == GraphArea[2])) {
                        temp = e.xany.window;
                    }
                    // メニューウインドウ生成
                    else if (e.xany.window == MenuBar) {
                        if (menu_flag == 0) {
                            MenuWin = XCreateSimpleWindow(dpy, win, (GRAPH_WIDTH + 30), 126, (WIN_WIDTH - (GRAPH_WIDTH + 50)), 32 * 3, 1, black, white);
                            Menu[0] = XCreateSimpleWindow(dpy, MenuWin, -2, (-2 + 0 * 32), (WIN_WIDTH - (GRAPH_WIDTH + 50) + 4), 36, 1, black, white);
                            Menu[1] = XCreateSimpleWindow(dpy, MenuWin, -2, (-2 + 1 * 32), (WIN_WIDTH - (GRAPH_WIDTH + 50) + 4), 36, 1, black, white);
                            Menu[2] = XCreateSimpleWindow(dpy, MenuWin, -2, (-2 + 2 * 32), (WIN_WIDTH - (GRAPH_WIDTH + 50) + 4), 36, 1, black, white);
                            XSelectInput(dpy, MenuWin, ButtonPressMask | ButtonReleaseMask);
                            XSelectInput(dpy, Menu[0], ButtonPressMask | ButtonReleaseMask);
                            XSelectInput(dpy, Menu[1], ButtonPressMask | ButtonReleaseMask);
                            XSelectInput(dpy, Menu[2], ButtonPressMask | ButtonReleaseMask);
                            XMapSubwindows(dpy, win);
                            XMapSubwindows(dpy, MenuWin);
                            DrawStringFormat(dpy, Menu[0], fs, gc, c, sort_type_text[0], strlen(sort_type_text[0]), 0, 0);
                            DrawStringFormat(dpy, Menu[1], fs, gc, c, sort_type_text[1], strlen(sort_type_text[1]), 0, 0);
                            DrawStringFormat(dpy, Menu[2], fs, gc, c, sort_type_text[2], strlen(sort_type_text[2]), 0, 0);
                            menu_flag = 1;
                        }

                    }
                    // メニューウインドウ破棄
                    else if ((e.xany.window == Menu[0]) || (e.xany.window == Menu[1]) || e.xany.window == Menu[2]) {
                        SortType = e.xany.window;
                        XDestroyWindow(dpy, Menu[0]);
                        XDestroyWindow(dpy, Menu[1]);
                        XDestroyWindow(dpy, Menu[2]);
                        XDestroyWindow(dpy, MenuWin);

                        if (SortType == Menu[0])
                            DrawStringFormat(dpy, GraphArea[1], fs, gc, c, sort_type_text[0], strlen(sort_type_text[0]), 0, 0);
                        else if (SortType == Menu[1])
                            DrawStringFormat(dpy, GraphArea[1], fs, gc, c, sort_type_text[1], strlen(sort_type_text[1]), 0, 0);
                        else if (SortType == Menu[2])
                            DrawStringFormat(dpy, GraphArea[1], fs, gc, c, sort_type_text[2], strlen(sort_type_text[2]), 0, 0);

                        DrawStringFormat(dpy, GraphArea[2], fs, gc, c, delay_text, strlen(delay_text), 0, 0);
                        menu_flag = 0;
                    }

                case KeyPress:
                    // テキストボックス操作
                    if (temp == TextBox || temp == GraphArea[2]) {
                        if (XLookupString((XKeyEvent *)&e, string, 9, &key, NULL) == 1) {
                            here[0] = string[0];
                            here[1] = '\0';
                            if (temp == TextBox)
                                NumConsole(dpy, TextBox, fs, gc, c, here, data_text, &pos0, pos0_lim, 1, -1, 0);
                            else
                                NumConsole(dpy, GraphArea[2], fs, gc, c, here, delay_text, &pos1, pos1_lim, 0, 0, 0);
                        }
                    }
            }
        } else {
            // イベント以外の処理
            DrawStringFormat(dpy, ExitButton, fs, gc, c, exit_button_text, strlen(exit_button_text), 0, 0);
            DrawStringFormat(dpy, StartButton, fs, gc, c, start_button_text, strlen(start_button_text), 0, 0);
            DrawStringFormat(dpy, GraphArea[1], fs, gc, c, graph_text[1], strlen(graph_text[1]), -1, 1);
            DrawStringFormat(dpy, GraphArea[2], fs, gc, c, graph_text[2], strlen(graph_text[2]), -1, 1);
            DrawStringFormat(dpy, GraphArea[3], fs, gc, c, graph_text[3], strlen(graph_text[3]), -1, 1);
            DrawStringFormat(dpy, GraphArea[4], fs, gc, c, graph_text[4], strlen(graph_text[4]), -1, 1);
            XFillArc(dpy, MenuBar, gc, -10, -3, 40, 40, (60 * 64), (60 * 64));
        }
    }
}

// 最大値選択法
void SelectionSort(Display *dpy, Window *win, XFontSet fs, GC gc, unsigned long *c, int *data, int *order, int size, int *count, int delay) {
    int i, j;
    DrawGraph(dpy, win, fs, gc, c, data, order, size, count, delay, 0);

    for (i = 0; i < size - 1; i++) {
        for (j = i + 1; j < size; j++) {
            if (data[i] > data[j]) {
                swap(&data[i], &data[j]);
                swap(&order[i], &order[j]);
                count[0]++;
                DrawGraph(dpy, win, fs, gc, c, data, order, size, count, delay, 1);
            }
        }
    }
}

// バブルソート
void BubbleSort(Display *dpy, Window *win, XFontSet fs, GC gc, unsigned long *c, int *data, int *order, int size, int *count, int delay) {
    int i, j;
    DrawGraph(dpy, win, fs, gc, c, data, order, size, count, delay, 0);

    for (i = 0; i < size - 1; i++) {
        for (j = 0; j < size - 1; j++) {
            if (data[j + 1] < data[j]) {
                swap(&data[j], &data[j + 1]);
                swap(&order[j], &order[j + 1]);
                count[0]++;
                DrawGraph(dpy, win, fs, gc, c, data, order, size, count, delay, 1);
            }
        }
    }
}

// クイックソート
void QuickSort(Display *dpy, Window *win, XFontSet fs, GC gc, unsigned long *c, int *data, int *order, int size, int *count, int delay) {
    DrawGraph(dpy, win, fs, gc, c, data, order, size, count, delay, 0);

    if (size < 2)
        return;

    int i, last = 1;

    for (i = 1; i < size; i++) {
        if (data[0] > data[i]) {
            if (last != i) {
                swap(&data[last], &data[i]);
                swap(&order[last], &order[i]);
                count[0]++;
                DrawGraph(dpy, win, fs, gc, c, data, order, size, count, delay, 1);
            }
            last++;
        }
    }
    swap(&data[0], &data[last - 1]);
    swap(&order[0], &order[last - 1]);
    count[0]++;
    DrawGraph(dpy, win, fs, gc, c, data, order, size, count, delay, 1);

    QuickSort(dpy, win, fs, gc, c, data, order, last, count, delay);
    QuickSort(dpy, win, fs, gc, c, (data + last), (order + last), (size - last), count, delay);

    DrawGraph(dpy, win, fs, gc, c, data, order, size, count, delay, 0);
}

// 棒グラフ描画
void DrawGraph(Display *dpy, Window *win, XFontSet fs, GC gc, unsigned long *c, int *data, int *order, int size, int *count, int delay, int flag) {
    if (flag != 0)
        usleep(delay * 1000);

    DrawNumFormat(dpy, win[2], fs, gc, c, delay, 0, 0);     // 遅延[ms]の描画
    DrawNumFormat(dpy, win[3], fs, gc, c, size, 0, 0);      // データサイズの描画
    DrawNumFormat(dpy, win[4], fs, gc, c, count[0], 0, 0);  // 交換回数の描画

    char str[32];
    int i, h, x, y, len, max;
    int w = 30, space = 20;

    // 画面外へ出ないようにする
    if ((w * size + space * (size - 1)) > (GRAPH_WIDTH - space * 4)) {
        w = GRAPH_WIDTH / size * 3.0 / 5.0;
        space = w * 2.0 / 3.0;
    }

    x = (GRAPH_WIDTH - (w * size + space * (size - 1))) / 2;
    y = GRAPH_HEIGHT;
    max = MaxArray(data, size);

    XClearWindow(dpy, win[0]);

    for (i = 0; i < size; i++) {
        h = (y - 30) * data[i] / max;
        XSetForeground(dpy, gc, c[order[i]]); // データと色を対応させる
        XFillRectangle(dpy, win[0], gc, (x + (w + space) * i), (y - h), w, h);
        XSetForeground(dpy, gc, c[1]);
        XDrawRectangle(dpy, win[0], gc, (x + (w + space) * i), (y - h), w, h);
        sprintf(str, "%d", data[i]);
        len = strlen(str);
        XmbDrawString(dpy, win[0], fs, gc, ((x + (w + space) * i) + (w - 8 * len) / 2), (y - h - 5), str, len);
    }
    XFlush(dpy);
}

// 文字列描画
void DrawStringFormat(Display *dpy, Window win, XFontSet fs, GC gc, unsigned long *c, char *str, int len, int flag_x, int flag_y) {
    int x, y, w, h;
    GetWinSize(dpy, win, &w, &h);

    if (flag_x > 0)          x = w - 8 * (len + 1);
    else if (flag_x == 0)    x = (w - 8 * len) / 2;
    else                     x = 8;

    if (flag_y > 0)          y = 24;
    else if (flag_y == 0)    y = h / 2 + 8;
    else                     y = h - 8;

    XSetForeground(dpy, gc, c[0]);
    XFillRectangle(dpy, win, gc, 0, (y - 17), w, 19);
    XSetForeground(dpy, gc, c[1]);
    XmbDrawString(dpy, win, fs, gc, x, y, str, len);
    XFlush(dpy);
}

// 数値描画
void DrawNumFormat(Display *dpy, Window win, XFontSet fs, GC gc, unsigned long *c, int num, int flag_x, int flag_y) {
    char str[32];
    sprintf(str, "%d", num);
    DrawStringFormat(dpy, win, fs, gc, c, str, strlen(str), flag_x, flag_y);
}

// テキストボックス処理
void NumConsole(Display *dpy, Window win, XFontSet fs, GC gc, unsigned long *c, char *letter, char *str, int *pos, int pos_lim, int space_flag, int flag_x, int flag_y) {
    int i, w, h, len;
    GetWinSize(dpy, win, &w, &h);
    len = strlen(str);

    // 数字(スペース)を入力
    if (('0' <= letter[0] && letter[0] <= '9' || (letter[0] == ' ' && space_flag))) {
        if (len < pos_lim) {
            for (i = len; i >= pos[0]; i--)
                str[i + 1] = str[i];
            str[pos[0]] = letter[0];
            pos[0]++;
        }
    }
    // Backspace
    else if (letter[0] == '\b') {
        if (pos[0] > 0) {
            for (i = pos[0]; i <= len; i++)
                str[i - 1] = str[i];
            str[len] = '\0';
            pos[0]--;
        }
    } 
    // Delete
    else if (letter[0] == 0x7f) {
        if (pos[0] >= 0) {
            for (i = pos[0]; i < len; i++)
                str[i] = str[i + 1];
            str[len] = '\0';
        }
    } 
    // カーソルを1つ右に移動
    else if (letter[0] == 'd') {
        if (pos[0] < len)
            pos[0]++;
    } 
    // カーソルを1つ左に移動
    else if (letter[0] == 'a') {
        if (pos[0] > 0)
            pos[0]--;
    }

    XClearWindow(dpy, win);
    XSetForeground(dpy, gc, c[1]);
    DrawStringFormat(dpy, win, fs, gc, c, str, strlen(str), flag_x, flag_y);
    DrawCursor(dpy, win, fs, gc, c, str, strlen(str), pos[0], flag_x, flag_y);
    XFlush(dpy);
}

// カーソル描画
void DrawCursor(Display *dpy, Window win, XFontSet fs, GC gc, unsigned long *c, char *str, int len, int pos, int flag_x, int flag_y) {
    int w, h, x, y;
    GetWinSize(dpy, win, &w, &h);

    if (flag_x > 0)          x = w - 8 * (len + 1);
    else if (flag_x == 0)    x = (w - 8 * len) / 2;
    else                     x = 8;

    if (flag_y > 0)          y = 24;
    else if (flag_y == 0)    y = h / 2 + 8;
    else                     y = h - 8;

    XSetForeground(dpy, gc, c[1]);
    XFillRectangle(dpy, win, gc, (x + 8 * pos), (y - 15), 1, 17);
    XFlush(dpy);
}

// カラー情報取得
void ColorSet(Display *dpy, char color_name[][20], unsigned long *c) {
    char color_name_all[COLOR_MAX][20];
    int i = 0, s, r, g, b;

    Colormap cmap;                   // カラーマップ構造体
    XColor c0, c1;                   // カラー構造体
    cmap = DefaultColormap(dpy, 0);  // カラーマップを得る

    while ((s = getchar()) != EOF) {
        if (s == '\n')
            scanf("%d %d %d %[^\n]", &r, &g, &b, color_name_all[i++]);
    }

    strcpy(color_name[0], "white");
    strcpy(color_name[1], "black");

    srand(time(NULL));

    for (i = 2; i < DATA_MAX; i++) {
        strcpy(color_name[i], color_name_all[rand() % COLOR_MAX]);
    }
    for (int i = 0; i < DATA_MAX; i++) {
        XAllocNamedColor(dpy, cmap, color_name[i], &c1, &c0);
        c[i] = c1.pixel;
    }
}

// ウインドウサイズ取得
void GetWinSize(Display *dpy, Window win, int *w, int *h) {
    XWindowAttributes a;
    XGetWindowAttributes(dpy, win, &a);  // ウインドウ情報取得
    w[0] = a.width;
    h[0] = a.height;
}

// スワップ
void swap(int *a, int *b) {
    int work;
    work = a[0];
    a[0] = b[0];
    b[0] = work;
}

// 文字列からint型配列へ変換
int StrToArray(char *str, int len, int *data, int size) {
    int i, n = 0, flag = 1;

    for (i = 0; i < len; i++) {
        if (flag == 1) {
            if (str[i] != ' ') {
                sscanf(str + i, "%d", &data[n]);
                n++;
                flag = 0;

                if (n >= size)
                    return n;
            }
        } else {
            if (str[i] == ' ')
                flag = 1;
        }
    }
    return n;
}

// 配列の最大値取得
int MaxArray(int *data, int size) {
    int i, max = data[0];
    for (i = 1; i < size; i++) {
        if (data[i] > max)
            max = data[i];
    }
    return max;
}
