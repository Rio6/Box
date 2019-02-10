#ifndef COLLISION_H
#define COLLISION_H

#include "vector.h"

typedef enum {
    UP = 0,
    LEFT,
    DOWN,
    RIGHT
} Direction;

typedef struct {
    Vector pos;
    Vector norm;
    float dist;
} Collision;

Collision collideBoxWall(Vector *pos, float boxW, float boxH, float rot, float wallW, float wallH, Direction dir);

#endif
