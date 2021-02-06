#include <ray.h>

#include <stdio.h>

void frame(Canvas *canvas) {
    RGBA color = (RGBA){ 0, 255, 0, 255 };
    for (int x = -canvas->width / 2; x < canvas->width / 2; x++) {
        for (int y = -canvas->height / 2; y < canvas->height / 2; y++) {
            canvas->put_pixel(canvas, x, y, color);
        }
    }
}
