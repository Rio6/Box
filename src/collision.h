#ifndef COLLISION_H
#define COLLISION_H

#include "vector.h"

typedef enum {
    UP = 0,
    LEFT,
    DOWN,
    RIGHT
} Direction;

Vector collideBoxWall(Vector *pos, float boxW, float boxH, float rot, Direction dir);

#endif
