#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <limits.h>
#include <sys/ioctl.h>

#define pixel_alloc(width, height) (Pixel*)malloc(width*height*sizeof(Pixel))
#define dot(a, b) a.x * b.x + a.y * b.y
#define perpendicular(v) (vec2){v.y, -v.x}
#define max(a, b) a>b?a:b
#define min(a, b) a<b?a:b

typedef struct {
    float x, y;
} vec2;

typedef struct {
    float x, y, z;
} vec3;

typedef struct {
    unsigned char r, g, b;
} Pixel;

typedef struct {
    int width, height;
    Pixel* pixels;
} Display;

/*
    Creates a `Pixel` with the specified `r` `g` and `b`.
*/
Pixel pixel(char r, char g, char b) {
    Pixel p = {0};
    p.r = r;
    p.g = g;
    p.b = b;
    return p;
}

/*
    Creates a display with the specified `width` and `height`.
*/
Display create_display(int width, int height) {
    Display d = {0};
    d.width = width;
    d.height = height;
    d.pixels = pixel_alloc(width, height);
    return d;
}

/*
    Resize `display` to the terminal size.
    Can result in a segmentation fault.
*/
void resize(Display* display) {
    struct winsize size;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
        
    if (size.ws_row != 2 * display->height) {
        (*display).height = 2 * size.ws_row;
        free((*display).pixels);
        (*display).pixels = pixel_alloc(display->width, display->height);
    }

    if (size.ws_col != display->width) {
        (*display).width = size.ws_col;
        free((*display).pixels);
        (*display).pixels = pixel_alloc(display->width, display->height);
    }
}

/*
    Waits 1 / `FPS` seconds.
*/
void tick(float FPS) {
    usleep(1000000 / FPS);
}

vec2 create_vec2(float x, float y) {
    vec2 v = {0};
    v.x = x;
    v.y = y;
    return v;
}

vec3 create_vec3(float x, float y, float z) {
    vec3 v = {0};
    v.x = x;
    v.y = y;
    v.z = z;
    return v;
}

/*
    Sets a pixel in the specified `display` to `pixel`.
*/
int set_pixel(Display display, int x, int y, Pixel pixel) {
    if (x >= display.width || x < 0 || y >= display.width || y < 0) 
        return 1;
    display.pixels[x+y*display.width] = pixel;
    return 0;
}

/*
    Frees all the pixel memory in the display.
    Also works as an exit code.
*/
int terminate(Display* display) {
    free(display->pixels);
    return 0;
}

/*
    Sets a line in the specified `display` to `pixel`.
*/
void set_line(Display display, int x1, int y1, int x2, int y2, Pixel pixel) {
    int dx = abs(x2 - x1);
    int dy = -abs(y2 - y1);
    
    int dir_x = x1 < x2 ? 1 : -1;
    int dir_y = y1 < y2 ? 1 : -1;
    int error = dx + dy;

    while (1) {
        set_pixel(display, x1, y1, pixel);
        int error_d = 2 * error;

        if (error_d >= dy) {
            if (x1 == x2) break;
            error += dy;
            x1 += dir_x;
        }

        if (error_d <= dx) {
            if (y1 == y2) break;
            error += dx;
            y1 += dir_y;
        }
    }
}

/*
    Sets a triangle in the specified `display` to `pixel`.
*/
void set_triangle(Display display, int x1, int y1, int x2, int y2, int x3, int y3, Pixel pixel) {
    int min_x = min(min(x1, x2), x3);
    int min_y = min(min(y1, y2), y3);
    int max_x = max(max(x1, x2), x3);
    int max_y = max(max(y1, y2), y3);

    vec2 ABperp = perpendicular(create_vec2(x2 - x1, y2 - y1));
    vec2 BCperp = perpendicular(create_vec2(x3 - x2, y3 - y2));
    vec2 CAperp = perpendicular(create_vec2(x1 - x3, y1 - y3));

    for (int j = min_y; j <= max_y; j++) {
        for (int i = min_x; i <= max_x; i++) {
            vec2 PA = create_vec2(i - x1, j - y1);
            vec2 PB = create_vec2(i - x2, j - y2);
            vec2 PC = create_vec2(i - x3, j - y3);

            if (dot(ABperp, PA) <= 0 && dot(BCperp, PB) <= 0 && dot(CAperp, PC) <= 0) {
                set_pixel(display, i, j, pixel);
            }
        }
    }
}

/*
    Sets a rectangle in the specified `display` to `pixel`.
*/
void set_rect(Display display, int x, int y, int width, int height, Pixel pixel) {
    for (int j = y; j < y + height; j++) {
        for (int i = x; i < x + width; i++) {
            set_pixel(display, i, j, pixel);
        }
    }
}

/*
    Draws `display` into the terminal.
*/
void render(Display display) {
    system("clear");
    for (int j = 0; j < display.height; j += 2) {
        for (int i = 0; i < display.width; i++) {
            Pixel pixel = display.pixels[i+j*display.width];
            Pixel next_pixel = display.pixels[i+(j+1)*display.width];
            printf("\033[38;2;%d;%d;%dm\033[48;2;%d;%d;%dm▀", 
                pixel.r, pixel.g, pixel.b,
                next_pixel.r, next_pixel.g, next_pixel.b
            );
        }
        printf("\033[0m\033[49m\n");
    }
}