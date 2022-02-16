//
// Created by tomokazu on 2022/02/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <wand/MagickWand.h>
#include <malloc.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <omp.h>
#include <math.h>
#include <time.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

void ThrowWandException(const MagickWand *wand) {
    char *description;
    ExceptionType *severity;
    description = MagickGetException(wand, severity);
    (void) fprintf(stderr, "%s %s %lu %s\n", GetMagickModule(), description);
    description = (char *) MagickRelinquishMemory(description);
    exit(-1);
}

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

    MagickExportImagePixels(magick_wand, 0, 0, image_width, image_height, "RGBA", CharPixel, blob);

    struct rgb {
        unsigned short red;
        unsigned short green;
        unsigned short blue;
        unsigned short alpha;
    };
    struct rgb *pixel_array = malloc(total_pixels * sizeof(struct rgb));

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

    Display *display;
    XInitThreads();
    printf("%d", omp_get_max_threads());
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

    int palette_size;
    if (DisplayWidth(display, screen) > DisplayHeight(display, screen)) {
        palette_size = DisplayWidth(display, screen);
    } else palette_size = DisplayHeight(display, screen);
    if (palette_size < image_width) {
        palette_size = image_width;
    } else if (palette_size < image_height) {
        palette_size = image_height;
    }

    Pixmap pixmap = XCreatePixmap(display, window, palette_size * 3, palette_size * 3,
                                  24);
    unsigned int color, count = 2;
    while (1) {
        XWindowAttributes windowAttributes;
        XGetWindowAttributes(display, window, &windowAttributes);
        window_width = windowAttributes.width;
        window_height = windowAttributes.height;
        //printf("%d %d %d %d\n", window_width, window_height, DisplayWidth(display, screen),
        //       DisplayHeight(display, screen));
        unsigned short black_cell = 100;
        unsigned short white_cell = 150;
        unsigned short cell_size = 20;
        for (int i = 0; i <= windowAttributes.width; i++) {
            for (int j = 0; j <= windowAttributes.height; j++) {
                color = (((i / cell_size) % 2 - (j / cell_size) % 2 + 2) % 2) *
                        (256 * 256 + 256 + 1) * black_cell + (256 * 256 + 256 + 1) * white_cell;
                XSetForeground(display, Multi_GC[omp_get_thread_num()], color);
                XDrawPoint(display, pixmap, Multi_GC[omp_get_thread_num()], i, j);
            }
        }
        for (int i = 0; i < image_width; i++) {
            for (int j = 0; j < image_height; j++) {
                color = pixel_array[j * image_width + i].red * 256 * 256 +
                        pixel_array[j * image_width + i].green * 256 +
                        pixel_array[j * image_width + i].blue;
                XSetForeground(display, Multi_GC[omp_get_thread_num()], color);
                XDrawPoint(display, pixmap, Multi_GC[omp_get_thread_num()],
                           i + (window_width - image_width) / 2,
                           j + (window_height - image_height) / 2);

            }
        }
        XCopyArea(display, pixmap, window, Multi_GC[omp_get_thread_num()], 0, 0, window_width, window_height, 0, 0);
    }


    return 0;
}

#pragma clang diagnostic pop