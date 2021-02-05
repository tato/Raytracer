#include <ray.h>

#include <stdio.h>

void fill_canvas(Canvas *canvas) {
    printf("Hello C!\n");
    for (unsigned int i; i < canvas->width * canvas->height; i++) {
        canvas->memory[i*3] = 255;
        canvas->memory[i*3 + 1] = 0;
        canvas->memory[i*3 + 2] = 0;
    }
}
