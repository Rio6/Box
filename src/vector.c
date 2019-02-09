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
