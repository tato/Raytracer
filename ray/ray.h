#ifndef RAY_H
#define RAY_Y

typedef struct {
    unsigned char *memory; // buffer of rgb pixels
    unsigned int width;
    unsigned int height;
} Canvas;

void fill_canvas(Canvas *canvas);

#endif
