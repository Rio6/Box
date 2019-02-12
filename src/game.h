#ifndef GAME_H
#define GAME_H

#include "SDL2/SDL.h"
#include "vector.h"

#define FPS 60
#define MAX_FINGERS 32
//#define DEBUG

typedef struct {
    int width;
    int height;
    SDL_Texture *tex;

    Vector pos;
    Vector vel;
    Vector force;
    Vector off; // offset from center to apply finger force

    float torq; // using "torq" as angular acceleration
    float angV;
    float rot;
    float mass;
    float inertia;
} Box;

typedef struct {
    Vector pos;
    int touch;
} Finger;

typedef struct {
    int width;
    int height;
    int running;

    Finger fingers[MAX_FINGERS];

    Box box;
    Vector grav;

    float gForce;
    float fForce;
    float airFric;
} Game;

void applyForce(Box *box, Vector* pos, Vector* force) {
    Vector relPos = *pos;
    vectorSub(&relPos, &box->pos);

    float mag = vectorMag(force);
    float angle = vectorAngle(force) - (vectorAngle(&relPos));
    float torq = mag * sin(angle) * vectorMag(&relPos);
    if(abs(torq) < .00001f) torq = 0;

    box->torq += torq;
    vectorAdd(&box->force, force);
}

#endif
