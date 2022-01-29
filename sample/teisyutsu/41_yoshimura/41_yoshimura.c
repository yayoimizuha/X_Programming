#include <X11/Xlib.h>
#include <stdio.h>
#include <math.h>
typedef float scalar;
typedef struct{
	scalar x;
	scalar y;
	scalar z;
}vector;
typedef struct{
	vector a;
	vector b;
	vector c;
}matrix;
typedef struct{
	scalar z;
	unsigned long c;
}subdisplay;
#define BORDER 2
#define WIDTH 500
#define HIGHT 500
#define header 13
#define BackgroundDepth 1000
#define KeyShift1 50
#define KeyShift2 62
#define KeyCtrl1 37
#define KeyCtrl2 105
#define KeyEnter 36
#define KeyW 25
#define KeyA 38
#define KeyS 39
#define KeyD 40
#define KeyUp 111
#define KeyLeft 113
#define KeyDown 116
#define KeyRight 114
#define KeyE 26
#define KeyQ 24
#define KeyR 27
#define KeyF 41
#define KeyZ 52
#define KeyX 53
int magnification=700;
int scan(Display *dpy, matrix mattri[128][128], matrix pointmat[128], unsigned long color[128][128], int q[128]);
void event(Display *dpy, int *key, int *button, int *mousex, int *mousey, int *f);
void move(int *slct, int  key, int  button, int x, int y, matrix mattri[128][128], matrix pointmat[128], int q[128], int set, int *f);
matrix asgn(scalar ax, scalar ay, scalar az, scalar bx, scalar by, scalar bz, scalar cx, scalar cy, scalar cz);
vector VMmulti(vector vec, matrix mat);
matrix MMmulti(matrix matA, matrix matB);
vector Vadd(vector vecA, vector vecB);
vector Vsbtr(vector vecA, vector vecB);
matrix Madd(matrix matA, matrix matB);
matrix Msbtr(matrix matA, matrix matB);
void prjctTri(matrix mat, subdisplay subdpy[WIDTH][HIGHT - header], unsigned long facecolor, unsigned long linecolor);
matrix rotate(scalar x, scalar y);
int maximum(int a, int b, int c, int lim);
unsigned long MyColor(Display *dpy, char *color);
int main(int argc, char **argv){
	Display *dpy;
	Window w;
	Window root;
	int screen;
	unsigned long black, white;
	GC gc;
	XFontSet fs;
	char **miss, *def;
	Colormap cmap;
	char hyouji[256] = {};
	XColor c0, c1;
	dpy = XOpenDisplay("");
	root = DefaultRootWindow (dpy);
	screen = DefaultScreen (dpy);
	white = WhitePixel (dpy, screen);
	black = BlackPixel (dpy, screen);
	cmap = DefaultColormap(dpy, 0);
	XAllocNamedColor(dpy, cmap, "green", &c1, &c0);
	w = XCreateSimpleWindow(dpy,  root,100, 100, WIDTH, HIGHT, BORDER, black, white);
	gc = XCreateGC(dpy, w, 0, NULL);

	vector pos = {};
	vector rotvec = {};
	matrix pointmat[128] = {};
	matrix mattri[128][128] = {};
	unsigned long color[128][128] = {};
	int i, j, q[128] = {}, slct = 0, set, key, button, mousex, mousey, x, y, f;
	subdisplay subdpy[WIDTH][HIGHT - header] = {};
	XMapWindow(dpy, w);
	XSelectInput(dpy, w, ButtonPressMask|ButtonReleaseMask|KeyPressMask|KeyReleaseMask|PointerMotionMask);

	set = scan(dpy, mattri, pointmat, color, q);
	while(1){
		XSetForeground(dpy, gc, black);
		XDrawLine(dpy, w, gc, 0, header - 1, WIDTH, header - 1);
		if(key != 0 || button != 0){
			for(i = 0; i < WIDTH; i++){
				for(j = 0; j < HIGHT - header; j++){
					subdpy[i][j].z = BackgroundDepth;
					subdpy[i][j].c = white;
				}
			}
			if(!XEventsQueued(dpy, QueuedAfterReading)){
				for(i = 0; i <= set; i++){
					for(j = 0; j < q[i]; j++){
						if(i == slct)
							prjctTri(mattri[i][j], subdpy, color[i][j], white);
						else
							prjctTri(mattri[i][j], subdpy, color[i][j], black);
					}
				}
				for(i = 0; i < WIDTH; i++){
					for(j = 0; j < HIGHT - header; j++){
						XSetForeground(dpy, gc, subdpy[i][j].c);
						XDrawPoint(dpy, w, gc, i, j + header);
					}
				}
			}
			XSetForeground(dpy, gc, black);
		}
		if(slct > set)
			sprintf(hyouji, "All  ");
		else
			sprintf(hyouji, "No.%2d", slct);
		XSetForeground(dpy, gc, white);
		XFillRectangle(dpy, w, gc, 0, 0, WIDTH, header - 1);
		XSetForeground(dpy, gc, black);
		XDrawString(dpy, w, gc, 5, 10, hyouji, 5);
		XFlush(dpy);
		x = mousex, y = mousey;
		event(dpy, &key, &button, &mousex, &mousey, &f);
		move(&slct, key, button, mousex - x, mousey - y, mattri, pointmat, q, set, &f);
		if(key == KeyEnter){
			for(i = 0;i <= set; i++){
				printf("1\n");
				printf("%f,%f,%f\n", pointmat[i].a.x, pointmat[i].a.y, pointmat[i].a.z);
				for(j = 0; j < q[i]; j++){
					printf("3\n%lu\n", color[i][j]);
					printf("%f,%f,%f\n", mattri[i][j].a.x, mattri[i][j].a.y, mattri[i][j].a.z);
					printf("%f,%f,%f\n", mattri[i][j].b.x, mattri[i][j].b.y, mattri[i][j].b.z);
					printf("%f,%f,%f\n", mattri[i][j].c.x, mattri[i][j].c.y, mattri[i][j].c.z);
				}
			}
			printf("0\n");
			return 0;
		}
	}
}


int scan(Display *dpy, matrix mattri[128][128], matrix pointmat[128], unsigned long color[128][128], int q[128]){
	int r, g, b, set = -1, com = 1;
	char rgb[15];
	while(com != 0){
		scanf("%d", &com);
		switch(com){
			case 1:
				set++;
				scanf("%f,%f,%f", &pointmat[set].a.x, &pointmat[set].a.y, &pointmat[set].a.z);
				pointmat[set].b = pointmat[set].a;
				pointmat[set].c = pointmat[set].a;
				break;
			case 2:
				scanf("%d,%d,%d", &r, &g, &b);
				scanf("%f,%f,%f", &mattri[set][q[set]].a.x, &mattri[set][q[set]].a.y, &mattri[set][q[set]].a.z);
				scanf("%f,%f,%f", &mattri[set][q[set]].b.x, &mattri[set][q[set]].b.y, &mattri[set][q[set]].b.z);
				scanf("%f,%f,%f", &mattri[set][q[set]].c.x, &mattri[set][q[set]].c.y, &mattri[set][q[set]].c.z);
				sprintf(rgb,"rgb:%02x/%02x/%02x", r, g, b);
				color[set][q[set]] = MyColor(dpy,rgb);
				q[set]++;
				break;
			case 3:
				scanf("%10lu", &color[set][q[set]]);
				scanf("%f,%f,%f", &mattri[set][q[set]].a.x, &mattri[set][q[set]].a.y, &mattri[set][q[set]].a.z);
				scanf("%f,%f,%f", &mattri[set][q[set]].b.x, &mattri[set][q[set]].b.y, &mattri[set][q[set]].b.z);
				scanf("%f,%f,%f", &mattri[set][q[set]].c.x, &mattri[set][q[set]].c.y, &mattri[set][q[set]].c.z);
				q[set]++;
		}
	}
	return set;
}
void event(Display *dpy, int *key, int *button, int *mousex, int *mousey, int *f){
	XEvent e;
	if(XEventsQueued(dpy, QueuedAfterReading)){
		XNextEvent(dpy, &e);
		switch (e.type){
			case KeyPress:
				*key = e.xkey.keycode;
				break ;
			case KeyRelease:
				*key = 0;
				*f = 0;
				break ;
			case ButtonPress:
				*button = e.xbutton.button;
				break ;
			case ButtonRelease:
				*button = 0;
				break ;
			case MotionNotify:
				*mousex = e.xmotion.x, *mousey = e.xmotion.y;
		}
	}
	return ;
}
void move(int *slct, int  key, int  button, int x, int y, matrix mattri[128][128], matrix pointmat[128], int q[128], int set, int *f){
	int i, j, z;
	matrix movemat, subpointmat;
	if(key == KeyCtrl1 || key == KeyCtrl2)
		y = 0;
	if(key == KeyShift1 || key == KeyShift2)
		x = 0;
	movemat = rotate(0, 0);
	if(button == 3)
		movemat = rotate(x / 50.0, y / 50.0);
	else
		movemat = rotate(0.02 * ((key == KeyRight) - (key == KeyLeft)), 0.02 * ((key == KeyDown) - (key == KeyUp)));
	if(*slct <= set){
		for(i = 0; i < q[*slct]; i++)
			mattri[*slct][i] = Madd(MMmulti(Msbtr(mattri[*slct][i], pointmat[*slct]), movemat), pointmat[*slct]);
	}else{
		subpointmat = asgn(0, 0, 0, 0, 0, 0, 0, 0, 0);
		for(i = 0; i <= set; i++){
			subpointmat = Madd(subpointmat, pointmat[i]);
		}
		subpointmat = MMmulti(subpointmat, asgn(1.0 / (set + 1), 0, 0, 0, 1.0 / (set + 1), 0, 0, 0, 1.0 / (set + 1)));
		for(j = 0; j <= set; j++){
			for(i = 0; i < q[j]; i++)
				mattri[j][i] = Madd(MMmulti(Msbtr(mattri[j][i], subpointmat), movemat), subpointmat);
			pointmat[j] = Madd(MMmulti(Msbtr(pointmat[j], subpointmat), movemat), subpointmat);
		}
	}
	if(button == 2){
		movemat = asgn(0, 0, -y, 0, 0, -y, 0, 0, -y);
		magnification += x;
	}else if(button == 1)
		movemat = asgn(x, y, 0, x, y, 0, x, y, 0);
	else{
		movemat = asgn((key == KeyD) - (key == KeyA), (key == KeyS) - (key == KeyW), (key == KeyR) - (key == KeyF), 0, 0, 0, 0, 0, 0);
		movemat.b = movemat.a;
		movemat.c = movemat.a;
	}
	magnification += (key == KeyZ) - (key == KeyX);
	if(*slct <= set){
		for(i = 0; i < q[*slct]; i++)
			mattri[*slct][i] = Madd(mattri[*slct][i], movemat);
		pointmat[*slct] = Madd(pointmat[*slct], movemat);
	}else{
		for(j = 0; j <= set; j++){
			for(i = 0; i < q[j]; i++)
				mattri[j][i] = Madd(mattri[j][i], movemat);
			pointmat[j] = Madd(pointmat[j], movemat);
		}
	}
	if(button == 4 || (key == KeyE && *f == 0)){
		*slct += 1;
		if(*slct > set + 1)
			*slct = 0;
		*f = 1;
	}
	if(button == 5 || (key == KeyQ && *f == 0)){
		*slct -= 1;
		if(*slct < 0)
			*slct = set + 1;
		*f = 1;
	}
	return ;
}
matrix asgn(scalar ax, scalar ay, scalar az, scalar bx, scalar by, scalar bz, scalar cx, scalar cy, scalar cz){
	matrix mat = {
		{ax, ay, az},
		{bx, by, bz},
		{cx, cy, cz}};
	return mat;
}
vector VMmulti(vector vec, matrix mat){
	vector subvec;
	subvec.x = vec.x * mat.a.x + vec.y * mat.b.x + vec.z * mat.c.x;
	subvec.y = vec.x * mat.a.y + vec.y * mat.b.y + vec.z * mat.c.y;
	subvec.z = vec.x * mat.a.z + vec.y * mat.b.z + vec.z * mat.c.z;
	return subvec;
}
matrix MMmulti(matrix matA, matrix matB){
	matrix matC, submat = matB;
	submat.a.y = matB.b.x;
	submat.a.z = matB.c.x;
	submat.b.x = matB.a.y;
	submat.b.z = matB.c.y;
	submat.c.x = matB.a.z;
	submat.c.y = matB.b.z;
	matC.a = VMmulti(matA.a, submat);
	matC.b = VMmulti(matA.b, submat);
	matC.c = VMmulti(matA.c, submat);
	return matC;
}
vector Vadd(vector vecA, vector vecB){
	vector vecC;
	vecC.x = vecA.x + vecB.x;
	vecC.y = vecA.y + vecB.y;
	vecC.z = vecA.z + vecB.z;
	return vecC;
}
vector Vsbtr(vector vecA, vector vecB){
	vector vecC;
	vecC.x = vecA.x - vecB.x;
	vecC.y = vecA.y - vecB.y;
	vecC.z = vecA.z - vecB.z;
	return vecC;
}
matrix Madd(matrix matA, matrix matB){
	matA.a = Vadd(matA.a, matB.a);
	matA.b = Vadd(matA.b, matB.b);
	matA.c = Vadd(matA.c, matB.c);
	return matA;
}
matrix Msbtr(matrix matA, matrix matB){
	matA.a = Vsbtr(matA.a, matB.a);
	matA.b = Vsbtr(matA.b, matB.b);
	matA.c = Vsbtr(matA.c, matB.c);
	return matA;
}
void prjctTri(matrix mat, subdisplay subdpy[WIDTH][HIGHT - header], unsigned long facecolor, unsigned long linecolor){
	scalar dis;
	int ax, bx, cx, ay, by, cy, i, j, pa, pb, pc, Mx, mx, My, my;
	scalar B, C, Z, det;
	dis = magnification / mat.a.z;
	ax = mat.a.x*dis+WIDTH / 2, ay = mat.a.y*dis + HIGHT / 2;
	dis = magnification / mat.b.z;
	bx = mat.b.x * dis + WIDTH / 2, by = mat.b.y * dis + HIGHT / 2;
	dis = magnification / mat.c.z;
	cx = mat.c.x * dis + WIDTH / 2, cy = mat.c.y * dis + HIGHT / 2;
	Mx = maximum(ax, bx, cx, WIDTH), mx = -maximum(-ax, -bx, -cx, 0), My = maximum(ay, by, cy, HIGHT - header), my = -maximum(-ay, -by, -cy, 0);
	for(i = mx; i < Mx; i++){
		for(j = my; j < My; j++){
			pa = (bx-ax) * (j-ay) - (by-ay) * (i-ax);
			pb = (cx-bx) * (j-by) - (cy-by) * (i-bx);
			pc = (ax-cx) * (j-cy) - (ay-cy) * (i-cx);
			if((pa * pb >= 0) && (pb * pc > 0)){
				det = (cy-ay) * (bx-ax) - (cx-ax) * (by-ay);
				B = ((cy-ay) * (i-ax) - (cx-ax) * (j-ay)) / det;
				C = (-(by-ay) * (i-ax) + (bx-ax) * (j-ay)) / det;
				Z = mat.a.z + (mat.b.z - mat.a.z) * B + (mat.c.z - mat.a.z) * C;
				if((Z < subdpy[i][j].z) && (Z > 300)){
					if((pa * pa < (bx-ax) * (bx-ax) + (by-ay) * (by-ay)) || (pb * pb < (cx-bx) * (cx-bx) + (cy-by) * (cy-by)) || (pc * pc < (ax-cx) * (ax-cx) + (ay-cy) * (ay-cy)) || (Z + 1 > subdpy[i][j].z))
						subdpy[i][j].c = linecolor;
					else
						subdpy[i][j].c = facecolor;
					subdpy[i][j].z = Z;
				}
			}
		}
	}
}
matrix rotate(scalar x, scalar y){
	matrix matA, matB;
	matA = asgn(cos(x), 0, -sin(x), 0, 1, 0, sin(x), 0, cos(x));
	matB = asgn(1, 0, 0, 0, cos(y), sin(y), 0, -sin(y), cos(y));
	return MMmulti(matA, matB);
}
int maximum(int a, int b, int c, int lim){
	scalar M = a;
	if(M < b)
		M = b;
	if(M < c)
		M = c;
	if(lim < M)
		M = lim;
	return M;
}
unsigned long MyColor(Display *dpy, char *color){
	Colormap cmap;
	XColor c0, c1;
	cmap = DefaultColormap(dpy, 0);
	XAllocNamedColor(dpy, cmap, color, &c1, &c0);
	return (c1.pixel);
}
