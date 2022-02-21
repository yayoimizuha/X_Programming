//
// Created by tomokazu on 2022/02/16.
//

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"
// https://daeudaeu.com/affine/#i-10

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

const struct rgb transparent = {0, 0, 0, 0};

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

    window = XCreateSimpleWindow(display, root, 0, 0, window_width, window_height, 2, black, white);

    GC Multi_GC[omp_get_max_threads() - 1];
    for (int i = 0; i < omp_get_max_threads() - 1; ++i) {
        Multi_GC[i] = XCreateGC(display, window, 0, NULL);
    }

    XMapWindow(display, window);

    Colormap colormap = XDefaultColormap(display, 0);

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

    unsigned int color, count = 2;
    while (1) {
        while (XPending(display)) {
            XNextEvent(display, &xEvent);
            if (xEvent.type == KeyPress) {
                keySym = XkbKeycodeToKeysym(display, xEvent.xkey.keycode, 0, 0);
                printf("%lu\n", keySym);
                keySym = XkbKeycodeToKeysym(display, xEvent.xkey.keycode, 1, 0);
                printf("%lu\n", keySym);
                keySym = XkbKeycodeToKeysym(display, xEvent.xkey.keycode, 0, 1);
                printf("%lu\n", keySym);
            }
        }

        printf("\n");
        XGetWindowAttributes(display, window, &windowAttributes);
        window_width = windowAttributes.width;
        window_height = windowAttributes.height;
        //printf("%d %d %d %d\n", window_width, window_height, DisplayWidth(display, screen),
        //       DisplayHeight(display, screen));
        unsigned short black_cell = 35;
        unsigned short white_cell = 220;
        unsigned short cell_size = 20;
        window_change = window_width != before_ww || window_height != before_wh;
        image_change = image_width != before_iw || image_height != before_ih;
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
                .ScaleX = 0.4,
                .ScaleY = 0.4,
                .DisplaceX = 0,
                .DisplaceY = 0,
                .Rotate_Angle = 0,
                .FlipX = False,
                .FlipY = True
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
        usleep(15000);

    }


    return 0;
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
    int oX, oY, ix, iy, rx, ry, i, j, k;
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
    struct rgb *affine_img, *affined_img, *test_img;
    affine_img = (struct rgb *) malloc(sizeof(struct rgb) * palette_size * palette_size);
    affined_img = (struct rgb *) malloc(sizeof(struct rgb) * palette_size * palette_size);
    if (affined_img == NULL || affine_img == NULL)exit(-1);
    for (int k = 0; k < palette_size * palette_size; k++) {
        affine_img[k].red = affine_img[k].green = affine_img[k].blue = affine_img[k].alpha = 50;
        affined_img[k].red = affined_img[k].green = affined_img[k].blue = affined_img[k].alpha = 60;
        affined_img[k].red = 200;
    }
    printf("Palette size:\t%lu\n", sizeof(struct rgb) * palette_size * palette_size);
    int ou, a, b;
    for (int i = 0; i < image_height; i++) {
        a = (palette_size - image_height) / 2 + i;
        for (int j = 0; j < image_width; j++) {
            b = (palette_size - image_width) / 2 + j;
            ou = a * palette_size + b;
            //printf("%u\t%d\n", ou, j * image_width + i);
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
    return affined_img;
}


#pragma clang diagnostic pop