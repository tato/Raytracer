#include <ray.h>

#include <stdio.h>

void fill_canvas(Canvas *canvas) {
    for (unsigned int i = 0; i < canvas->width * canvas->height; i++) {
        canvas->memory[i*4]     = 0;
        canvas->memory[i*4 + 1] = 255;
        canvas->memory[i*4 + 2] = 0;
        canvas->memory[i*4 + 3] = 255;
    }
}

void put_pixel(
        Canvas *canvas,
        unsigned int x,
        unsigned int y,
        unsigned int color
) {
    unsigned int mx = canvas->width / 2 + x;
    unsigned int my = canvas->height / 2 + y; 
    unsigned int p = my * canvas->width + mx; 
    canvas->memory[p*4 + 0] = (unsigned char)(color >> 24);
    canvas->memory[p*4 + 1] = (unsigned char)(color >> 16);
    canvas->memory[p*4 + 2] = (unsigned char)(color >> 8);
    canvas->memory[p*4 + 3] = (unsigned char)(color >> 0);
}
