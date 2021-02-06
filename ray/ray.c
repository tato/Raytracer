#include <ray.h>

#include <stdbool.h>
#include <math.h>

#define INF 1000000.f

typedef struct {
    float x, y, z;
} V3;

typedef struct {
    V3 center;
    float radius;
    RGBA color;
} Sphere;

typedef struct {
    bool init;
    Canvas *canvas;
    float viewport_width, viewport_height, viewport_distance;
    V3 camera_origin;
    Sphere spheres[3]; // todo stb array 
} World;

float dot(V3 a, V3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

V3 canvas_to_viewport(World *world, int cx, int cy) {
    V3 v;
    v.x = cx * (world->viewport_width/world->canvas->width);
    v.y = cy * (world->viewport_height/world->canvas->height);
    v.z = world->viewport_distance;
    return v;
}

void intersect_ray_sphere(
        V3 o, V3 d, Sphere *sphere, // in
        float *t1, float *t2 // out
) {
    float r = sphere->radius;

    V3 co = (V3){
        o.x - sphere->center.x,
        o.y - sphere->center.y,
        o.z - sphere->center.z
    };

    float a = dot(d, d);
    float b = 2 * dot(co, d);
    float c = dot(co, co) - r*r;

    float discriminant = b*b - 4*a*c;
    if (discriminant < 0.f) {
        *t1 = INF;
        *t2 = INF;
        return;
    }

    *t1 = (-b + sqrtf(discriminant)) / (2*a);
    *t2 = (-b - sqrtf(discriminant)) / (2*a);
}

RGBA trace_ray(World *world, V3 d, float t_min, float t_max) {
    float closest_t = INF;
    Sphere *closest_sphere = NULL;

    for (int i = 0; i < 3; i++) {
        Sphere *sphere = &world->spheres[i];

        float t1, t2;
        intersect_ray_sphere(world->camera_origin, d, sphere, &t1, &t2);

        if (t1 >= t_min && t1 <= t_max && t1 < closest_t) {
            closest_t = t1;
            closest_sphere = sphere;
        }
        if (t2 >= t_min && t2 <= t_max && t2 < closest_t) {
            closest_t = t2;
            closest_sphere = sphere;
        }
    }

    if (closest_sphere == NULL) {
        return (RGBA){ 255, 255, 255, 255 };
    }
    return closest_sphere->color;
}

void frame(Canvas *canvas) {
    static World world = {0};
    if (!world.init) {
        world.viewport_width = 1.0f;
        world.viewport_height = 1.0f;
        world.viewport_distance = 1.0f;
        world.camera_origin = (V3){ 0.f, 0.f, 0.f };

        world.spheres[0].center = (V3){ 0.f, -1.f, 3.f };
        world.spheres[0].radius = 1.f;
        world.spheres[0].color = (RGBA){255, 0, 0, 255};

        world.spheres[1].center = (V3){ 2.f, 0.f, 4.f };
        world.spheres[1].radius = 1.f;
        world.spheres[1].color = (RGBA){0, 0, 255, 255};

        world.spheres[2].center = (V3){ -2.f, 0.f, 4.f };
        world.spheres[2].radius = 1.f;
        world.spheres[2].color = (RGBA){0, 255, 0, 255};
    }
    world.canvas = canvas;

    for (int x = -canvas->width / 2; x < canvas->width / 2; x++) {
        for (int y = -canvas->height / 2; y < canvas->height / 2; y++) {

            V3 v = canvas_to_viewport(&world, x, y);

            RGBA color = trace_ray(&world, v, 1.f, INF);

            canvas->put_pixel(canvas, x, y, color);
        }
    }
}

