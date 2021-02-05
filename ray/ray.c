#include <ray.h>

#include <stdio.h>

void fill_canvas(Canvas *canvas) {
    for (unsigned int i; i < canvas->width * canvas->height; i++) {
        canvas->memory[i*4]     = 0;
        canvas->memory[i*4 + 1] = 255;
        canvas->memory[i*4 + 2] = 0;
        canvas->memory[i*4 + 3] = 255;
    }
}
