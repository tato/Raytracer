#ifndef RAY_H
#define RAY_H

typedef unsigned char u8;
typedef int i32;
typedef struct { u8 r, g, b, a; } RGBA;

typedef void PutPixelFn(void *, i32, i32, RGBA);

typedef struct {
    u8 *memory; // buffer of rgb pixels
    i32 width, height;
    PutPixelFn *put_pixel;
} Canvas;


void frame(Canvas *canvas);

#endif
