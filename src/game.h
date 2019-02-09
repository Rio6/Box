#ifndef GAME_H
#define GAME_H

#include "SDL2/SDL.h"
#include "vector.h"

#define MAX_FINGERS 32

typedef struct {
    int width;
    int height;
    SDL_Texture *tex;

    Vector pos;
    Vector vel;
    Vector accl;
    Vector off; // offset from center to apply finger force
    float torq;
    float rot;
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
} Game;

#endif
