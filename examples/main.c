#include <math.h>
#include <graphics.h>

int main() {
    Display display = create_display(80, 50);

    for (int frames = 0; ; frames++) {
        for (int j = 0; j < display.height; j++) {
            for (int i = 0; i < display.width; i++) {
                set_pixel(display, i, j, pixel(i * (255 / display.width), j * (255 / display.height), 0));
            }
        }

        set_line(display, 0, 0, 20, frames, pixel(0, 0, 0));
        set_rect(display, 20, 30, frames, 5, pixel(0, 0, 0));
        set_triangle(display, 20, 49, 25, 40, 30 + frames, 49, pixel(0, 0, 0));

        // resize(&display);
        render(display);
        tick(20);
    }

    return terminate(&display);
}