#include <ray.h>

#define STB_DS_IMPLEMENTATION
#include <stb_ds.h>

#include <stdio.h>
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

typedef enum {
    AMBIENT, POINT, DIRECTIONAL
} LightType;

typedef struct {
    LightType type;
    float intensity;
    V3 position;
    V3 direction;
} Light;

typedef struct {
    bool init;
    Canvas *canvas;
    float viewport_width, viewport_height, viewport_distance;
    V3 camera_origin;
    Sphere *spheres; 
    Light *lights;
} World;

float dot(V3 a, V3 b) {
    return a.x*b.x + a.y*b.y + a.z*b.z;
}

V3 v3add(V3 a, V3 b) {
    return (V3){ a.x + b.x, a.y + b.y, a.z + b.z };
}
V3 v3sub(V3 a, V3 b) {
    return (V3){ a.x - b.x, a.y - b.y, a.z - b.z };
}
V3 v3mul(V3 a, float t) {
    return (V3){ a.x*t, a.y*t, a.z*t };
}
V3 v3div(V3 a, float t) {
    return (V3){ a.x/t, a.y/t, a.z/t };
}
float v3len(V3 v) {
    return sqrtf(v.x*v.x + v.y*v.y + v.z*v.z);
}
RGBA rgbamul(RGBA a, float t) {
    return (RGBA){ a.r*t, a.g*t, a.b*t, a.a*t };
}


V3 canvas_to_viewport(World *world, int cx, int cy) {
    V3 v;
    v.x = cx * (world->viewport_width/world->canvas->width);
    v.y = cy * (world->viewport_height/world->canvas->height);
    v.z = world->viewport_distance;
    return v;
}


float compute_lighting(World *world, V3 p, V3 n) {
    float result = 0.f;
    for (int i = 0; i < arrlen(world->lights); i++) {
        Light *light = &world->lights[i];
        if (light->type == AMBIENT) {
            result += light->intensity;
        } else {
            V3 l;
            if (light->type == POINT) {
                l = v3sub(light->position, p);
            } else {
                l = light->direction;
            }

            float n_dot_l = dot(n, l);
            if (n_dot_l > 0.f) {
                result += light->intensity * n_dot_l / (v3len(n) * v3len(l));
            }
        }
    }
    return result;
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

    for (int i = 0; i < arrlen(world->spheres); i++) {
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

    V3 p = v3add(world->camera_origin, v3mul(d, closest_t));
    V3 n = v3sub(p, closest_sphere->center);
    n = v3div(n, v3len(n));
    return rgbamul(closest_sphere->color, compute_lighting(world, p, n));
}


void frame(Canvas *canvas) {
    static World world = {0};
    if (!world.init) {
        world.init = true;

        world.viewport_width = 1.0f;
        world.viewport_height = 1.0f;
        world.viewport_distance = 1.0f;
        world.camera_origin = (V3){ 0.f, 0.f, 0.f };

        Sphere sphere = {};
        sphere.center = (V3){ 0.f, -1.f, 3.f };
        sphere.radius = 1.f;
        sphere.color = (RGBA){255, 0, 0, 255};
        arrput(world.spheres, sphere);

        sphere.center = (V3){ 2.f, 0.f, 4.f };
        sphere.radius = 1.f;
        sphere.color = (RGBA){0, 0, 255, 255};
        arrput(world.spheres, sphere);

        sphere.center = (V3){ -2.f, 0.f, 4.f };
        sphere.radius = 1.f;
        sphere.color = (RGBA){0, 255, 0, 255};
        arrput(world.spheres, sphere);

        sphere.center = (V3){ 0.f, -5001.f, 0.f };
        sphere.radius = 5000.f;
        sphere.color = (RGBA){255, 255, 0, 255};
        arrput(world.spheres, sphere);

        Light ambient = {};
        ambient.type = AMBIENT;
        ambient.intensity = .2f;
        arrput(world.lights, ambient);

        Light point = {};
        point.type = POINT;
        point.intensity = 0.6f;
        point.position = (V3){ 2.f, 1.f, 0.f };
        arrput(world.lights, point);

        Light directional = {};
        directional.type = DIRECTIONAL;
        directional.intensity = .2f;
        directional.direction = (V3){ 1.f, 4.f, 4.f };
        arrput(world.lights, directional);
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

