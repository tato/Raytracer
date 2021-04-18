#include <ray.h>
#include <stretchy_buffer.h>

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
    float specular;
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
    return (RGBA){
        fmin(fmax(0, a.r*t), 255),
        fmin(fmax(0, a.g*t), 255),
        fmin(fmax(0, a.b*t), 255),
        fmin(fmax(0, a.a*t), 255),
    };
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


void closest_intersection(World *world, V3 o, V3 d, float t_min, float t_max, Sphere **closest_sphere, float *closest_t) {
    *closest_t = INF;
    *closest_sphere = NULL;
    for (int i = 0; i < sb_count(world->spheres); i++) {
        Sphere *sphere = &world->spheres[i];

        float t1, t2;
        intersect_ray_sphere(o, d, sphere, &t1, &t2);

        if (t1 >= t_min && t1 <= t_max && t1 < *closest_t) {
            *closest_t = t1;
            *closest_sphere = sphere;
        }
        if (t2 >= t_min && t2 <= t_max && t2 < *closest_t) {
            *closest_t = t2;
            *closest_sphere = sphere;
        }
    }
}


float compute_lighting(World *world, V3 p, V3 n, V3 v, float s) {
    float result = 0.f;
    for (int i = 0; i < sb_count(world->lights); i++) {
        Light *light = &world->lights[i];
        if (light->type == AMBIENT) {
            result += light->intensity;
        } else {
            V3 l;
            float t_max;
            if (light->type == POINT) {
                l = v3sub(light->position, p);
                t_max = 1.f;
            } else {
                l = light->direction;
                t_max = INF;
            }

            Sphere *shadow_sphere;
            float shadow_t;
            closest_intersection(world, p, l, 0.001, t_max, &shadow_sphere, &shadow_t);
            if (shadow_sphere != NULL) {
                continue;
            }

            // diffuse
            float n_dot_l = dot(n, l);
            if (n_dot_l > 0.f) {
                result += light->intensity * n_dot_l / (v3len(n) * v3len(l));
            }

            // specular
            if (s != -1.f) {
                V3 r = v3mul(n, 2);
                r = v3mul(r, dot(n, l));
                r = v3sub(r, l);

                float r_dot_v = dot(r, v);
                if (r_dot_v > 0.f) {
                    float specular_i = pow(r_dot_v/(v3len(r)*v3len(v)), s);
                    result += light->intensity * specular_i;
                }
            }
        }
    }
    return result;
}


RGBA trace_ray(World *world, V3 d, float t_min, float t_max) {
    Sphere *closest_sphere;
    float closest_t;
    closest_intersection(world, world->camera_origin, d, t_min, t_max, &closest_sphere, &closest_t);


    if (closest_sphere == NULL) {
        return (RGBA){ 255, 255, 255, 255 };
    }

    V3 p = v3add(world->camera_origin, v3mul(d, closest_t));
    V3 n = v3sub(p, closest_sphere->center);
    n = v3div(n, v3len(n));
    return rgbamul(closest_sphere->color, compute_lighting(world, p, n, v3mul(d, -1), closest_sphere->specular));
}


void frame(Canvas *canvas) {
    static World world = {0};
    if (!world.init) {
        world.init = true;

        world.viewport_width = 1.0f;
        world.viewport_height = 1.0f;
        world.viewport_distance = 1.0f;
        world.camera_origin = (V3){ 0.f, 0.f, 0.f };

        Sphere sphere = {0};
        sphere.center = (V3){ 0.f, -1.f, 3.f };
        sphere.radius = 1.f;
        sphere.color = (RGBA){255, 0, 0, 255};
        sphere.specular = 500;
        sb_push(world.spheres, sphere);

        sphere.center = (V3){ 2.f, 0.f, 4.f };
        sphere.radius = 1.f;
        sphere.color = (RGBA){0, 0, 255, 255};
        sphere.specular = 500;
        sb_push(world.spheres, sphere);

        sphere.center = (V3){ -2.f, 0.f, 4.f };
        sphere.radius = 1.f;
        sphere.color = (RGBA){0, 255, 0, 255};
        sphere.specular = 10;
        sb_push(world.spheres, sphere);

        sphere.center = (V3){ 0.f, -5001.f, 0.f };
        sphere.radius = 5000.f;
        sphere.color = (RGBA){255, 255, 0, 255};
        sphere.specular = 1000;
        sb_push(world.spheres, sphere);

        Light ambient = {0};
        ambient.type = AMBIENT;
        ambient.intensity = .2f;
        sb_push(world.lights, ambient);

        Light point = {0};
        point.type = POINT;
        point.intensity = 0.6f;
        point.position = (V3){ 2.f, 1.f, 0.f };
        sb_push(world.lights, point);

        Light directional = {0};
        directional.type = DIRECTIONAL;
        directional.intensity = .2f;
        directional.direction = (V3){ 1.f, 4.f, 4.f };
        sb_push(world.lights, directional);
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

