#include <math.h>

#include "vector.h"

Vector *vectorAdd(Vector* a, Vector* b) {
    a->x += b->x;
    a->y += b->y;
    return a;
}

Vector *vectorSub(Vector* a, Vector* b) {
    a->x -= b->x;
    a->y -= b->y;
    return a;
}

Vector *vectorScale(Vector* a, float b) {
    a->x *= b;
    a->y *= b;
    return a;
}

Vector *vectorZero(Vector* a) {
    a->x = 0;
    a->y = 0;
    return a;
}

Vector *vectorRotate(Vector *v, float rot) {
    float x = cos(rot) * v->x - sin(rot) * v->y;
    float y = cos(rot) * v->y + sin(rot) * v->x;
    v->x = x; v->y = y;
    return v;
}

float vectorMag(Vector *v) {
    return sqrt(v->x * v->x + v->y * v->y);
}

float vectorAngle(Vector *v) {
    return atan2(v->y, v->x);
}
