#pragma clang diagnostic push
#pragma ide diagnostic ignored "openmp-use-default-none"
//
// Created by tomokazu on 2022/02/16.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

#include <stdio.h>
#include <stdlib.h>
#include <wand/MagickWand.h>
#include <malloc.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/Xutil.h>
#include <omp.h>
#include <math.h>
#include <time.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

void ThrowWandException(const MagickWand *wand) {
    char *description;
    ExceptionType *severity;
    description = MagickGetException(wand, severity);
    (void) fprintf(stderr, "%s %s %lu %s\n", GetMagickModule(), description);
    description = (char *) MagickRelinquishMemory(description);
    printf("%s", description);
    exit(-1);
}

void DestroyProcess(Display *display);

struct rgb {
    unsigned short red;
    unsigned short green;
    unsigned short blue;
    unsigned short alpha;
};
struct affine_func {
    double ScaleX;
    double ScaleY;
    double DisplaceX;
    double DisplaceY;
    double Rotate_Angle;
    bool FlipX;
    bool FlipY;
};

struct rgb alpha_marge(int x, int y, struct rgb color,
                       unsigned int image_width, unsigned int image_height,
                       unsigned int window_width, unsigned int window_height,
                       unsigned short black_cell, unsigned short white_cell, unsigned short cell_size);

struct rgb *process_img(struct rgb *base_img, struct affine_func,
                        unsigned int palette_size, unsigned int image_width, unsigned int image_height);

int main(int argc, char **argv) {

    MagickBooleanType status;

    MagickWand *magick_wand;

    if (argc != 2) {
        (void) fprintf(stdout, "Usage: %s image thumbnail\n", argv[0]);
        exit(0);
    }
    /*
      Read an image.
    */
    MagickWandGenesis();
    magick_wand = NewMagickWand();
    status = MagickReadImage(magick_wand, argv[1]);

    if (status == MagickFalse) ThrowWandException(magick_wand);

    size_t image_width = MagickGetImageWidth(magick_wand);
    size_t image_height = MagickGetImageHeight(magick_wand);
    size_t total_pixels = image_height * image_width;
    unsigned char *blob = malloc(total_pixels * sizeof(unsigned char) * 4);
    if (blob == NULL) {
        printf("memory error.\n");
        return -1;
    }

    MagickExportImagePixels(magick_wand, 0, 0, image_width, image_height, "RGBA", CharPixel, blob);


    struct rgb *pixel_array = malloc(total_pixels * sizeof(struct rgb));
    if (pixel_array == NULL) {
        printf("memory error.\n");
        return -1;
    }

    for (int i = 0; i < total_pixels; ++i) {
        pixel_array[i].red = blob[i * 4 + 0];
        pixel_array[i].green = blob[i * 4 + 1];
        pixel_array[i].blue = blob[i * 4 + 2];
        pixel_array[i].alpha = blob[i * 4 + 3];
    }

    printf("image width: %zu\n", image_width);
    printf("image height: %zu\n", image_height);
    printf("pixel array size(count,struct size): %lu(%lu,%lu)\n",
           malloc_usable_size(pixel_array),
           malloc_usable_size(pixel_array) / sizeof(struct rgb),
           sizeof(struct rgb));
    DestroyMagickWand(magick_wand);
    MagickWandTerminus();
    free(blob);

    /******************    Load Image   *********************/

    unsigned int window_width = image_width + 80;
    unsigned int window_height = image_height + 80;
    unsigned int before_ww, before_wh, before_iw, before_ih;

    Display *display;
    XInitThreads();
    printf("Thread: %d\n", omp_get_max_threads());
    omp_set_num_threads(omp_get_max_threads());
    Window window;
    Window root;
    KeySym keySym;
    XEvent xEvent;
    int screen;
    unsigned long white, black;
    display = XOpenDisplay("");
    root = XDefaultRootWindow(display);
    screen = DefaultScreen(display);
    white = WhitePixel(display, screen);
    black = BlackPixel(display, screen);
    char ScaleBuf[100], MoveBuf[200], RotateBuf[100];

    window = XCreateSimpleWindow(display, root, 0, 0, window_width, window_height, 2, black, white);

    GC Multi_GC[omp_get_max_threads() - 1];
    for (int i = 0; i < omp_get_max_threads() - 1; ++i) {
        Multi_GC[i] = XCreateGC(display, window, 0, NULL);
    }

    XMapWindow(display, window);


    size_t palette_size;
    bool image_change = True, window_change = True;
    if (DisplayWidth(display, screen) > DisplayHeight(display, screen)) {
        palette_size = DisplayWidth(display, screen);
    } else palette_size = DisplayHeight(display, screen);
    if (palette_size < image_width) {
        palette_size = image_width;
    } else if (palette_size < image_height) {
        palette_size = image_height;
    }

    Pixmap pixmap = XCreatePixmap(display, window, palette_size * 3, palette_size * 3, 24);
    Pixmap back_pixmap = XCreatePixmap(display, window, palette_size * 3, palette_size * 3, 24);
    XWindowAttributes windowAttributes;

    XSelectInput(display, window, ButtonPressMask | ButtonReleaseMask | KeyPressMask);
    int UserDisplaceX = 0, UserDisplaceY = 0, UserAngle = 0;
    double UserScaleX = 1, UserScaleY = 1;
    bool UserChange;
    bool UserFlipX, UserFlipY;
    int input_length = 1;
    char *input = (char *) malloc(sizeof(char) * input_length);
    XAutoRepeatOff(display);
    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);
    XSetWMProtocols(display, window, &wm_delete_window, 1);

    XFontSet fs;
    char **missing, *def_return;
    int missing_count, i;


    fs = XCreateFontSet(display, "-*-*-medium-r-normal-*-32-*-*-*-*-*-*-*", &missing, &missing_count, &def_return);
    if (missing_count != 0) {
        for (i = 0; i < missing_count; i++) {
            printf("missing 0: %d\t%s\n", i, missing[i]);
        }
        if (def_return != NULL) {
            printf("Default font: %s\n", def_return);
        }
    } else {
        printf("No missing font\n");
    }

    unsigned int color;
    while (1) {
        UserChange = False;
        while (XPending(display)) {
            XNextEvent(display, &xEvent);
            printf("time:\t%lu\n", xEvent.xkey.time);

            printf("type:\t %d\n", xEvent.type);
            if (xEvent.type == KeyPress) {
                input_length++;
                if (realloc(input, sizeof(char) * input_length) == NULL)exit(-1);
                UserChange = True;
                keySym = XkbKeycodeToKeysym(display, xEvent.xkey.keycode, 0, 0);
                printf("0,0\t%lu\n", keySym);
                keySym = XkbKeycodeToKeysym(display, xEvent.xkey.keycode, 1, 0);
                printf("1,0\t%lu\n", keySym);
                keySym = XkbKeycodeToKeysym(display, xEvent.xkey.keycode, 0, 1);
                printf("0,1\t%lu\n", keySym);
                printf("state:\t%d\n", xEvent.xkey.state);

                printf("\n");
                if (XkbKeycodeToKeysym(display, xEvent.xkey.keycode, 0, 0) == 65361) {
                    if (xEvent.xkey.state == ShiftMask) {
                        UserDisplaceX -= 300;
                    } else {
                        UserDisplaceX -= 50;
                    }
                }

                if (XkbKeycodeToKeysym(display, xEvent.xkey.keycode, 0, 0) == 65363) {
                    if (xEvent.xkey.state == ShiftMask) {
                        UserDisplaceX += 300;
                    } else {
                        UserDisplaceX += 50;
                    }
                }
                if (XkbKeycodeToKeysym(display, xEvent.xkey.keycode, 0, 0) == 65364) {
                    if (xEvent.xkey.state == ShiftMask) {
                        UserDisplaceY -= 300;
                    } else {
                        UserDisplaceY -= 50;
                    }
                }
                if (XkbKeycodeToKeysym(display, xEvent.xkey.keycode, 0, 0) == 65362) {
                    if (xEvent.xkey.state == ShiftMask) {
                        UserDisplaceY += 300;
                    } else {
                        UserDisplaceY += 50;
                    }

                }
                if (XkbKeycodeToKeysym(display, xEvent.xkey.keycode, 0, 0) == 102) {
                    if (xEvent.xkey.state == ControlMask) {
                        UserFlipX = !UserFlipX;
                    } else if (xEvent.xkey.state == 8) {
                        UserFlipY = !UserFlipY;
                    }
                }
                XLookupString((XKeyEvent *) &xEvent, input, 9, &keySym, 0);
                printf("input char: %c\n", input[0]);
                if (input[0] == '+' && xEvent.xkey.state == ShiftMask) {
                    UserScaleX *= 1.25;
                    UserScaleY *= 1.25;
                } else if (input[0] == '+' && xEvent.xkey.state == 5) {
                    UserScaleX *= 1.25;
                } else if (input[0] == '+' && xEvent.xkey.state == 9) {
                    UserScaleY *= 1.25;
                }

                if (input[0] == '-' && xEvent.xkey.state == 0) {
                    UserScaleX *= 0.8;
                    UserScaleY *= 0.8;
                } else if (input[0] == '-' && xEvent.xkey.state == 4) {
                    UserScaleX *= 0.8;
                } else if (input[0] == '-' && xEvent.xkey.state == 8) {
                    UserScaleY *= 0.8;
                }


                if (input[0] == '=') {
                    UserDisplaceX = UserDisplaceY = 0;
                    UserScaleX = UserScaleY = 1;
                    UserFlipX = UserFlipY = False;
                    UserAngle = 0;

                }
                if (input[0] == '1')UserAngle += 15;
                if (input[0] == '2')UserAngle += 5;
                if (input[0] == '3')UserAngle -= 5;
                if (input[0] == '4')UserAngle -= 15;

                if (input[0] == 'q') DestroyProcess(display);

            }
            if (xEvent.type == ClientMessage) {
                if (xEvent.xclient.data.l[0] == wm_delete_window)DestroyProcess(display);
            }

        }
        snprintf(ScaleBuf, 100, "X Scale: %f    Y Scale: %f", UserScaleX, UserScaleY);
        snprintf(MoveBuf, 100, "X Displace: %d    Y Displace:    %d", UserDisplaceX, UserDisplaceY);
        snprintf(RotateBuf, 100, "Angle(angle): %d", UserAngle);

        XGetWindowAttributes(display, window, &windowAttributes);
        window_width = windowAttributes.width;
        window_height = windowAttributes.height;

        unsigned short black_cell = 35;
        unsigned short white_cell = 220;
        unsigned short cell_size = 20;

        window_change = window_width != before_ww || window_height != before_wh;
        image_change = image_width != before_iw || image_height != before_ih || UserChange;

        if (window_change) {
            printf("[%ld]\tWindow Changed\n", time(NULL));
            for (int i = 0; i <= windowAttributes.width; i++) {
                for (int j = 0; j <= windowAttributes.height; j++) {
                    color = (((i / cell_size) % 2 - (j / cell_size) % 2 + 2) % 2) *
                            (256 * 256 + 256 + 1) * black_cell + (256 * 256 + 256 + 1) * white_cell;
                    XSetForeground(display, Multi_GC[omp_get_thread_num()], color);
                    XDrawPoint(display, back_pixmap, Multi_GC[omp_get_thread_num()], i, j);
                }
            }
        }

        struct affine_func affineFunc = {
                .ScaleX = UserScaleX,
                .ScaleY = UserScaleY,
                .DisplaceX = UserDisplaceX,
                .DisplaceY = UserDisplaceY,
                .Rotate_Angle = UserAngle,
                .FlipX = UserFlipX,
                .FlipY = UserFlipY
        };

        if (image_change || window_change) {
            if (image_change) printf("[%ld]\tImage Changed\n", time(NULL));
            struct rgb *manipulated_array = process_img(pixel_array, affineFunc,
                                                        palette_size, image_width, image_height);

            for (int i = 0; i < palette_size; i++) {
                for (int j = 0; j < palette_size; j++) {

                    struct rgb mixed_color = alpha_marge(i, j, manipulated_array[i * palette_size + j],
                                                         image_width, image_height,
                                                         window_width, window_height,
                                                         black_cell, white_cell, cell_size);

                    color = mixed_color.red * 256 * 256 +
                            mixed_color.green * 256 +
                            mixed_color.blue;

                    XSetForeground(display, Multi_GC[omp_get_thread_num()], color);
                    XDrawPoint(display, pixmap, Multi_GC[omp_get_thread_num()], j, i);

                }
            }

            free(manipulated_array);

            XCopyArea(display, back_pixmap, window, Multi_GC[omp_get_thread_num()],
                      0, 0,
                      window_width, window_height,
                      0, 0);

            XCopyArea(display, pixmap, window, Multi_GC[omp_get_thread_num()],
                      0, 0,
                      palette_size, palette_size,
                      (int) (window_width - palette_size) / 2, (int) (window_height - palette_size) / 2);
        }

        before_ww = window_width, before_wh = window_height, before_iw = image_width, before_ih = image_height;
        XSetForeground(display, Multi_GC[0], 0x0);
        XmbDrawString(display, window, fs, Multi_GC[0], 10, 30, ScaleBuf, (int) strlen(ScaleBuf));
        XmbDrawString(display, window, fs, Multi_GC[0], 10, 60, MoveBuf, (int) strlen(MoveBuf));
        XmbDrawString(display, window, fs, Multi_GC[0], 10, 90, RotateBuf, (int) strlen(RotateBuf));


        usleep(15000);
    }
}

struct rgb alpha_marge(int x, int y, struct rgb color,
                       unsigned int image_width, unsigned int image_height,
                       unsigned int window_width, unsigned int window_height,
                       unsigned short black_cell, unsigned short white_cell, unsigned short cell_size) {

    x += (int) (window_width - image_width) / 2;
    y += (int) (window_height - image_height) / 2;

    struct rgb mixed_color;

    mixed_color.red = (color.alpha * color.red +
                       (255 - color.alpha) *
                       ((((x / cell_size) % 2 - (y / cell_size) % 2 + 2) % 2) * black_cell + white_cell))
                      / 255;
    mixed_color.green = (color.alpha * color.green +
                         (255 - color.alpha) *
                         ((((x / cell_size) % 2 - (y / cell_size) % 2 + 2) % 2) * black_cell + white_cell))
                        / 255;
    mixed_color.blue = (color.alpha * color.blue +
                        (255 - color.alpha) *
                        ((((x / cell_size) % 2 - (y / cell_size) % 2 + 2) % 2) * black_cell + white_cell))
                       / 255;
    mixed_color.alpha = 0;

    return mixed_color;
}

struct rgb *process_img(struct rgb *base_img, struct affine_func option,
                        unsigned int palette_size, unsigned int image_width, unsigned int image_height) {

    int oX, oY, ix, iy, i, j, k;
    double rx, ry;
    int PosMove = (int) palette_size / 2;

    double Pos[3][3] = {
            {1, 0, PosMove},
            {0, 1, PosMove},
            {0, 0, 1}
    };

    if (option.FlipX == True)option.ScaleX *= -1;
    if (option.FlipY == True)option.ScaleY *= -1;

    double Resize[3][3] = {
            {option.ScaleX, 0,             0},
            {0,             option.ScaleY, 0},
            {0,             0,             1}
    };

    double Pos_Resize[3][3];
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            Pos_Resize[i][j] = 0;
            for (k = 0; k < 3; k++) {
                Pos_Resize[i][j] += Pos[i][k] * Resize[k][j];
            }
        }
    }

    double Move[3][3] = {
            {1, 0, option.DisplaceX},
            {0, 1, -option.DisplaceY},
            {0, 0, 1}
    };

    double Pos_Resize_Move[3][3];
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            Pos_Resize_Move[i][j] = 0;
            for (k = 0; k < 3; k++) {
                Pos_Resize_Move[i][j] += Pos_Resize[i][k] * Move[k][j];
            }
        }
    }

    double rotate = option.Rotate_Angle;
    rotate = rotate * M_PI / 180;
    double Rotate[3][3] = {
            {cos(rotate),  sin(rotate), 0},
            {-sin(rotate), cos(rotate), 0},
            {0,            0,           1}
    };

    double Pos_Resize_Move_Rotate[3][3];
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            Pos_Resize_Move_Rotate[i][j] = 0;
            for (k = 0; k < 3; k++) {
                Pos_Resize_Move_Rotate[i][j] += Pos_Resize_Move[i][k] * Rotate[k][j];
            }
        }
    }

    double DePos[3][3] = {
            {1, 0, -PosMove},
            {0, 1, -PosMove},
            {0, 0, 1}
    };

    double Matrix[3][3];
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            Matrix[i][j] = 0;
            for (k = 0; k < 3; k++) {
                Matrix[i][j] += Pos_Resize_Move_Rotate[i][k] * DePos[k][j];
            }
        }
    }

    double det = Matrix[0][0] * Matrix[1][1] * Matrix[2][2]
                 + Matrix[0][1] * Matrix[1][2] * Matrix[2][0]
                 + Matrix[0][2] * Matrix[1][0] * Matrix[2][1]
                 - Matrix[0][2] * Matrix[1][1] * Matrix[2][0]
                 - Matrix[0][1] * Matrix[1][0] * Matrix[2][2]
                 - Matrix[0][0] * Matrix[1][2] * Matrix[2][1];
    if (det == 0) {
        printf("divided zero!!!!\n");
        return 0;
    }

    double Inv_Matrix[3][3];
    Inv_Matrix[0][0] = (Matrix[1][1] * Matrix[2][2] - Matrix[1][2] * Matrix[2][1]) / det;
    Inv_Matrix[0][1] = -(Matrix[0][1] * Matrix[2][2] - Matrix[0][2] * Matrix[2][1]) / det;
    Inv_Matrix[0][2] = (Matrix[0][1] * Matrix[1][2] - Matrix[0][2] * Matrix[1][1]) / det;
    Inv_Matrix[1][0] = -(Matrix[1][0] * Matrix[2][2] - Matrix[1][2] * Matrix[2][0]) / det;
    Inv_Matrix[1][1] = (Matrix[0][0] * Matrix[2][2] - Matrix[0][2] * Matrix[2][0]) / det;
    Inv_Matrix[1][2] = -(Matrix[0][0] * Matrix[1][2] - Matrix[0][2] * Matrix[1][0]) / det;
    Inv_Matrix[2][0] = (Matrix[1][0] * Matrix[2][1] - Matrix[1][1] * Matrix[2][0]) / det;
    Inv_Matrix[2][1] = -(Matrix[0][0] * Matrix[2][1] - Matrix[0][1] * Matrix[2][0]) / det;
    Inv_Matrix[2][2] = (Matrix[0][0] * Matrix[1][1] - Matrix[0][1] * Matrix[1][0]) / det;

    struct rgb *affine_img, *affined_img;

    affine_img = (struct rgb *) malloc(sizeof(struct rgb) * palette_size * palette_size);
    affined_img = (struct rgb *) malloc(sizeof(struct rgb) * palette_size * palette_size);
    if (affined_img == NULL || affine_img == NULL)exit(-1);

#pragma omp parallel for
    for (int k = 0; k < palette_size * palette_size; k++) {
        affine_img[k].red = affine_img[k].green = affine_img[k].blue = affine_img[k].alpha = 50;
        affined_img[k].red = affined_img[k].green = affined_img[k].blue = affined_img[k].alpha = 60;
        affined_img[k].red = 200;
    }

    printf("Palette size:\t%lu\n", sizeof(struct rgb) * palette_size * palette_size);

    int ou, a, b;
#pragma omp parallel for firstprivate(i, j, ou, a, b)
    for (int i = 0; i < image_height; i++) {
        a = ((int) palette_size - (int) image_height) / 2 + i;
        for (int j = 0; j < image_width; j++) {
            b = ((int) palette_size - (int) image_width) / 2 + j;
            ou = a * (int) palette_size + b;
            affine_img[ou].red = base_img[i * image_width + j].red;
            affine_img[ou].green = base_img[i * image_width + j].green;
            affine_img[ou].blue = base_img[i * image_width + j].blue;
            affine_img[ou].alpha = base_img[i * image_width + j].alpha;
        }
    }

    printf("Affine Matrix.\n");
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            printf("%f\t", Inv_Matrix[i][j]);
        }
        printf("\n");
    }
    printf("\nInvert Matrix.\n");
    for (i = 0; i < 3; i++) {
        for (j = 0; j < 3; j++) {
            printf("%f\t", Matrix[i][j]);
        }
        printf("\n");
    }

    double I[3][1], O[3][1], vecA[3], vecB[3], inner_product;
#pragma omp parallel for private(oY, oX, I, O, i, j, k, rx, ix, ry, iy, vecA, vecB, inner_product)
    for (oY = 0; oY < palette_size; oY++) {
        for (oX = 0; oX < palette_size; oX++) {
            I[0][0] = oX, I[1][0] = oY, I[2][0] = 1;
            for (i = 0; i < 3; i++) {
                for (j = 0; j < 1; j++) {
                    for (k = 0; k < 3; k++) {
                        vecA[k] = Inv_Matrix[i][k];
                        vecB[k] = I[k][j];
                    }
                    inner_product = 0;
                    for (k = 0; k < 3; k++) {
                        inner_product += vecA[k] * vecB[k];
                    }
                    O[i][j] = inner_product;
                }
            }
            rx = O[0][0];


            ix = (int) (rx + 0.5);
            if (ix >= palette_size || ix < 0) continue;
            ry = O[1][0];

            iy = (int) (ry + 0.5);
            if (iy >= palette_size || iy < 0)continue;
            affined_img[oX + oY * palette_size] = affine_img[ix + iy * palette_size];
        }
    }
    free(affine_img);
    return affined_img;
}

void DestroyProcess(Display *display) {
    XSetCloseDownMode(display, DestroyAll);
    XCloseDisplay(display);
    printf("Quit...\n");
    exit(0);
}


#pragma clang diagnostic pop
#pragma clang diagnostic pop