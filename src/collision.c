#include <math.h>

#include "collision.h"
#include "vector.h"

#include "SDL2/SDL.h"

Vector collideBoxWall(Vector *pos, float boxW, float boxH, float rot, Direction dir) {

    int quad = ((int) (-rot / (M_PI/2)) - dir) % 4;
    if(quad < 0) quad += 4;

    Vector closest;
    switch(quad) {
        case 0:
            closest.x = -boxW / 2.0f;
            closest.y = -boxH / 2.0f;
            break;
        case 1:
            closest.x = boxW / 2.0f;
            closest.y = -boxH / 2.0f;
            break;
        case 2:
            closest.x = boxW / 2.0f;
            closest.y = boxH / 2.0f;
            break;
        case 3:
            closest.x = -boxW / 2.0f;
            closest.y = boxH / 2.0f;
            break;
    }

    vectorRotate(&closest, rot);
    vectorAdd(&closest, pos);

    return closest;
}
