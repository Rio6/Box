#ifndef GAME_H
#define GAME_H

#define FPS 60
#define MAX_FINGERS 32
//#define DEBUG

#include "SDL.h"
#include "vector.h"

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
    SDL_FingerID id;
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

void applyForce(Box *box, Vector* pos, Vector* force);
Finger *findFingerById(SDL_FingerID id, Finger *fingers, int fingers_c);
Finger *findFreeFinger(Finger *fingers, int fingers_c);

#endif
