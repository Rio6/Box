#ifndef VECTOR_H
#define VECTOR_H

#include <math.h>
#include "vector.h"

typedef struct {
    float x;
    float y;
} Vector;

inline Vector *vectorAdd(Vector* a, Vector* b) {
    a->x += b->x;
    a->y += b->y;
    return a;
}

inline Vector *vectorSub(Vector* a, Vector* b) {
    a->x -= b->x;
    a->y -= b->y;
    return a;
}

inline Vector *vectorMult(Vector* a, Vector* b) {
    a->x *= b->x;
    a->y *= b->y;
    return a;
}

inline Vector *vectorScale(Vector* a, float b) {
    a->x *= b;
    a->y *= b;
    return a;
}

inline Vector *vectorZero(Vector* a) {
    a->x = 0;
    a->y = 0;
    return a;
}

inline Vector *vectorRotate(Vector *v, float rot) {
    float x = cos(rot) * v->x - sin(rot) * v->y;
    float y = cos(rot) * v->y + sin(rot) * v->x;
    v->x = x; v->y = y;
    return v;
}

inline float vectorMag(Vector *v) {
    return sqrt(v->x * v->x + v->y * v->y);
}

inline float vectorAngle(Vector *v) {
    return atan2(v->y, v->x);
}

inline float vectorDot(Vector *a, Vector* b) {
    return a->x * b->x + a->y * b->y;
}

inline float vectorCross(Vector *a, Vector *b) {
    return a->x * b->y - a->y * b->x;
}

#endif
