//
// Created by tomokazu on 2022/02/16.
//

// https://daeudaeu.com/affine/#i-10

#include <stdio.h>
#include <stdlib.h>
#include <wand/MagickWand.h>
#include <malloc.h>
#include <X11/Xlib.h>
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
    double A0;
    double A1;
    double A2;
    double A3;
    double dx;
    double dy;
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


    unsigned int color, count = 2;
    while (1) {
        XSelectInput(display, window, ButtonPressMask | ButtonReleaseMask | KeyPressMask);
        XWindowAttributes windowAttributes;
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
        struct affine_func affineFunc;
        double r = (0 * M_PI) / 180;
        affineFunc.A0 = cos(r);
        affineFunc.A1 = -sin(r);
        affineFunc.A2 = sin(r);
        affineFunc.A3 = cos(r);
        affineFunc.A0 = 2;
        affineFunc.A1 = 0;
        affineFunc.A2 = 0;
        affineFunc.A3 = 2;
        affineFunc.dx = -(int) image_width;
        affineFunc.dy = 0;
        (double) (window_height * 2 - image_height) / 2;
        if (image_change || window_change) {
            if (image_change) printf("[%ld]\tImage Changed\n", time(NULL));
            struct rgb *manipulated_array = process_img(pixel_array, affineFunc,
                                                        window_width, window_height,
                                                        image_width, image_height);
            for (int i = 0; i < image_width; i++) {
                for (int j = 0; j < image_height; j++) {
                    struct rgb mixed_color = alpha_marge(i, j, manipulated_array[j * image_width + i],
                                                         image_width, image_height,
                                                         window_width, window_height,
                                                         black_cell, white_cell, cell_size);
                    color = mixed_color.red * 256 * 256 +
                            mixed_color.green * 256 +
                            mixed_color.blue;
                    XSetForeground(display, Multi_GC[omp_get_thread_num()], color);
                    XDrawPoint(display, pixmap, Multi_GC[omp_get_thread_num()], i, j);

                }
            }
            XCopyArea(display, back_pixmap, window, Multi_GC[omp_get_thread_num()],
                      0, 0,
                      window_width, window_height,
                      0, 0);

            XCopyArea(display, pixmap, window, Multi_GC[omp_get_thread_num()],
                      0, 0,
                      image_width, image_height,
                      (int) (window_width - image_width) / 2, (int) (window_height - image_height) / 2);
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
    double A[4], iA[4];
    A[0] = option.A0, A[1] = option.A1, A[2] = option.A2, A[3] = option.A3;
    int m = (int) (option.dx * A[0]), n = (int) (option.dy * A[3]);
    int oX, oY, noX, noY, ix, iy, rx, ry, c;
    double det = A[0] * A[3] - A[1] * A[2];
    if (det == 0) {
        printf("divided zero.\n");
        return 0;
    }
    iA[0] = A[3] / det;
    iA[1] = A[1] / det;
    iA[2] = A[2] / det;
    iA[3] = A[0] / det;
    struct rgb *affine_img, *affined_img, *test_img;
    affine_img = malloc(sizeof(struct rgb) * palette_size * palette_size);
    affined_img = malloc(sizeof(struct rgb) * palette_size * palette_size);

    // for (int i = 0; i < window_width; i++) {
    //     for (int j = 0; j < window_height; j++) {
    //         affine_img[j * window_width + i] = transparent;
    //         affined_img[j * window_width + i] = transparent;
    //     }
    // }
    // for (int i = 0; i < image_width; i++) {
    //     for (int j = 0; j < image_height; j++) {
    //         if (((int) window_height - j) <= 0 || ((int) window_width - i) <= 0)continue;
    //         if (((int) window_height - j) > window_height || ((int) window_width - i) > window_width) continue;
    //         affine_img[(j + ((int) window_height - j) / 2) * image_width +
    //                    i + ((int) window_width - i) / 2] = base_img[j * image_width + i];
    //     }
    // }
    // return affine_img;
    test_img = malloc(sizeof(struct rgb) * image_height * image_width * 10);
    for (oY = 0; oY < image_height; oY++) {
        noY = oY - (int) image_height / 2;
        for (oX = 0; oX < image_width; oX++) {
            noX = oX - (int) image_width / 2;

            rx = (double) noX * iA[0] + (double) noY * iA[1] - (double) m * iA[0] - (double) n * iA[1] -
                 image_width / 2;

            ix = (int) (rx + 0.5);
            if (ix >= image_width || ix < 0) continue;
            ry = (double) noX * iA[2] + (double) noY * iA[3] - (double) m * iA[2] - (double) n * iA[3] +
                 image_height / 2;
            iy = (int) (ry + 0.5);
            if (iy >= image_height || iy < 0)continue;
            test_img[oX + oY * image_width] = base_img[ix + iy * image_width];
        }
    }
    return test_img;


}

//struct rgb *process_img(struct rgb *base_img, unsigned int base_width, unsigned int base_height) {
//    struct rgb OUT_RANGE = {255, 255, 255, 255};
//    int i, j;
//    int nRow, nCol;
//    double dRow, dCol;
//    int nOutRow, nOutCol, nInRow, nInCol;
//    int nSample;
//    double daAffCoef[6];
//    struct rgb *return_array = malloc(base_width * base_height * sizeof(struct rgb));
//    for (i = 0; i < nOutRow; i++) {
//        for (j = 0; j < nOutCol; j++) {
//            dCol = daAffCoef[0] * (double) j + daAffCoef[1] * (double) i + daAffCoef[2];
//            dRow = daAffCoef[3] * (double) j + daAffCoef[4] * (double) i + daAffCoef[5];
//            nRow = (int) floor(dRow + 0.5);
//            nCol = (int) floor(dCol + 0.5);
//
//            //範囲のチェック
//            if (nCol < 0 || nCol >= nInCol || nRow < 0 || nRow >= nInRow) {
//                for (int k = 0; k < nSample; k++) {
//                    return_array[i * nOutCol * nSample + j * nSample + k] = OUT_RANGE;
//                }
//            } else {
//                for (int k = 0; k < nSample; k++) {
//                    return_array[i * nOutCol * nSample + j * nSample + k] =
//                            base_img[nRow * nInCol * nSample + nCol * nSample + k];
//                }
//            }
//        }
//    }
//    return return_array;
//}

#pragma clang diagnostic pop