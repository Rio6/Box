#ifndef VECTOR_H
#define VECTOR_H

typedef struct {
    float x;
    float y;
} Vector;

Vector *vectorAdd(Vector*, Vector*);
Vector *vectorSub(Vector*, Vector*);
Vector *vectorScale(Vector*, float);
Vector *vectorZero(Vector*);
Vector *vectorRotate(Vector*, float);
float vectorMag(Vector*);
float vectorAngle(Vector*);

#endif
