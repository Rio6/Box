#include <math.h>

#include "collision.h"
#include "vector.h"

Collision collideBoxWall(Vector *pos, float boxW, float boxH, float rot, float wallW, float wallH, Direction dir) {

    int quad = ((int) (-rot / (M_PI/2)) - dir) % 4;
    if(quad < 0) quad += 4;

    Vector closest = {0, 0};
    if(fmod(-rot, (M_PI/2)) == 0) {
        // two corners with same distance
        switch(quad) {
            case 0:
                closest.y = -boxH / 2.0f;
                break;
            case 1:
                closest.x = boxW / 2.0f;
                break;
            case 2:
                closest.y = boxH / 2.0f;
                break;
            case 3:
                closest.x = -boxW / 2.0f;
                break;
        }
    } else {
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
    }

    vectorRotate(&closest, rot);
    vectorAdd(&closest, pos);

    float dist;
    Vector norm = {0, 0};
    switch(dir) {
        case UP:
            dist = closest.y;
            norm.y = 1;
            break;
        case LEFT:
            dist = closest.x;
            norm.x = 1;
            break;
        case DOWN:
            dist = wallH - closest.y;
            norm.y = -1;
            break;
        case RIGHT:
            dist = wallW - closest.x;
            norm.x = -1;
            break;
    }

    Collision coll = {
        .pos = closest,
        .norm = norm,
        .dist = dist
    };
    return coll;
}
