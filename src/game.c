/*
 * Author: Rio
 * Date: 2017/05/21
 */

#include <math.h>

#include "SDL.h"
#include "game.h"
#include "vector.h"
#include "collision.h"

int main(int argc, char *argv[]) {
    Game game = {
        .width = 600,
        .height = 900,
        .running = 1,
        .box = {
            .width = 80,
            .height = 20,
            .pos = {game.width / 2.0f, game.height / 2.0f},
            .off = {10, 0},
            .mass = 1,
            .inertia = game.box.mass * (game.box.width * game.box.width + game.box.height * game.box.height) / 12
        },
        .gForce = .6,
        .fForce = 1.2,
        .airFric = 1
    };

    // log everything
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

    // init and create things
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init: %s", SDL_GetError());
        return 1;
    }

    // display size
    SDL_DisplayMode display;

#if defined(__ANDROID__) || defined(__IPHONEOS__)
    // only scale window to screen size on android
    if(SDL_GetDesktopDisplayMode(0, &display) != 0) {
        // failed
#endif

        display.w = game.width;
        display.h = game.height;

#if defined(__ANDROID__) || defined(__IPHONEOS__)
    }
#endif

    SDL_Window *win = SDL_CreateWindow("Box",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            display.w, display.h, SDL_WINDOW_SHOWN);
    if(!win) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create window: %s", SDL_GetError());
        return 1;
    }

    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if(!rend) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create renderer: %s", SDL_GetError());
        return 1;
    }

    // configure SDL
    SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "1");
    SDL_SetHint(SDL_HINT_ANDROID_SEPARATE_MOUSE_AND_TOUCH, "1");
    SDL_RenderSetScale(rend, (float) display.w / game.width, (float) display.h / game.height);
    SDL_EnableScreenSaver();

    // joystick (and device accelerator)
    int jsticks_c = SDL_NumJoysticks();
    SDL_Joystick **jsticks = NULL;
    if(jsticks_c > 0) {
        jsticks = malloc(sizeof(SDL_Joystick*) * jsticks_c);
        for(int i = 0; i < jsticks_c; i++) {
            jsticks[i] = SDL_JoystickOpen(i);
            if(!jsticks[i]) {
                SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to open joystick %d: %s", i, SDL_GetError());
            }
        }
    }

    // Box texture
    game.box.tex = SDL_CreateTexture(rend,
            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
            game.box.width, game.box.height);
    SDL_SetRenderTarget(rend, game.box.tex);
    SDL_SetRenderDrawColor(rend, 255, 221, 0, 255);
    SDL_Rect boxRect = {0, 0, game.box.width, game.box.height};
    SDL_RenderFillRect(rend, &boxRect);
    SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
    SDL_Point off = {game.box.off.x + game.box.width / 2.0f, game.box.off.y + game.box.height / 2.0f};
    SDL_RenderDrawLine(rend, off.x - 5, off.y - 5, off.x + 5, off.y + 5);
    SDL_RenderDrawLine(rend, off.x + 5, off.y - 5, off.x - 5, off.y + 5);
    SDL_SetRenderTarget(rend, NULL);

    // loop
    while(game.running) {
        int startTick = SDL_GetTicks();
        // events
        SDL_Event eve;
        while(SDL_PollEvent(&eve)) {
            switch(eve.type) {
                case SDL_KEYDOWN:
                    if(eve.key.repeat) break;
                    switch(eve.key.keysym.sym) {
                        case SDLK_UP:
                            game.grav.y -= game.gForce;
                            break;
                        case SDLK_DOWN:
                            game.grav.y += game.gForce;
                            break;
                        case SDLK_LEFT:
                            game.grav.x -= game.gForce;
                            break;
                        case SDLK_RIGHT:
                            game.grav.x += game.gForce;
                            break;
                    }
                    break;
                case SDL_KEYUP:
                    switch(eve.key.keysym.sym) {
                        case SDLK_UP:
                            game.grav.y += game.gForce;
                            break;
                        case SDLK_DOWN:
                            game.grav.y -= game.gForce;
                            break;
                        case SDLK_LEFT:
                            game.grav.x += game.gForce;
                            break;
                        case SDLK_RIGHT:
                            game.grav.x -= game.gForce;
                            break;
                        case SDLK_AC_BACK:
                            game.running = 0;
                            break;
#ifdef DEBUG
                        case SDLK_f:
                            game.box.vel.x = 0;
                            game.box.vel.y = 0;
                            game.box.angV = 0;
                            break;
#endif
                    }
                    break;
                case SDL_JOYAXISMOTION:
                    {
                        float value = (float) eve.jaxis.value / 32767; // normalize the value
                        switch(eve.jaxis.axis) {
                            case 0: // x axis
                                game.grav.x = value * game.gForce;
                                break;
                            case 1: // y axis
                                game.grav.y = value * game.gForce;
                                break;
                        }
                        break;
                    }
                case SDL_MOUSEBUTTONDOWN:
                    game.fingers[0].pos.x = eve.button.x;
                    game.fingers[0].pos.y = eve.button.y;
                    game.fingers[0].touch = 1;
                    break;
                case SDL_MOUSEMOTION:
                    if(eve.motion.state != 0) {
                        game.fingers[0].pos.x = eve.motion.x;
                        game.fingers[0].pos.y = eve.motion.y;
                    }
                    break;
                case SDL_MOUSEBUTTONUP:
                    game.fingers[0].touch = 0;
                    break;
                case SDL_FINGERDOWN:
                case SDL_FINGERMOTION:
                    {
                        SDL_FingerID id = eve.tfinger.fingerId;
                        Finger *finger = findFingerById(id, game.fingers, MAX_FINGERS);
                        if(!finger) finger = findFreeFinger(game.fingers, MAX_FINGERS);
                        if(finger) {
                            finger->pos.x = eve.tfinger.x * game.width;
                            finger->pos.y = eve.tfinger.y * game.height;
                            finger->touch = 1;
                        }
                    }
                    break;
                case SDL_FINGERUP:
                    {
                        Finger *finger = findFingerById(eve.tfinger.fingerId, game.fingers, MAX_FINGERS);
                        if(finger) finger->touch = 0;
                        break;
                    }
                case SDL_QUIT:
                    game.running = 0;
                    break;
            }
        }

        // game logic
        // apply gravity
        vectorAdd(&game.box.force, &game.grav);

        // apply other forces
        for(int i = 0; i < MAX_FINGERS; i++) {
            Finger finger = game.fingers[i];
            if(finger.touch) {

                Vector pos = game.box.off;
                vectorRotate(&pos, game.box.rot);
                vectorAdd(&pos, &game.box.pos);

                Vector force = {(finger.pos.x - pos.x) / game.width, (finger.pos.y - pos.y) / game.width};
                vectorScale(&force, game.fForce);

                applyForce(&game.box, &pos, &force);
            }
        }

        // calculate velocity and position
        vectorAdd(&game.box.vel, vectorScale(&game.box.force, 1 / game.box.mass));
        vectorScale(&game.box.vel, game.airFric);
        vectorAdd(&game.box.pos, &game.box.vel);

        // and angular speed and rotation
        game.box.angV += game.box.torq / game.box.inertia;
        game.box.angV *= game.airFric;
        game.box.rot += game.box.angV;

        // collision
        for(Direction dir = UP; dir <= RIGHT; dir++) {
            Collision coll = collideBoxWall(&game.box.pos, game.box.width, game.box.height, game.box.rot, game.width, game.height, dir);
            if(coll.dist < 0) {
                // Impulse
                Vector off = coll.pos;
                vectorSub(&off, &game.box.pos);
                Vector vel = {
                    game.box.vel.x - game.box.angV * off.y,
                    game.box.vel.y + game.box.angV * off.x
                };

                float contSpeed = vectorDot(&vel, &coll.norm);
                if(contSpeed < 0) {
                    float r = vectorCross(&off, &coll.norm);
                    float massSum = 1 / game.box.mass + r * r / game.box.inertia;
                    float f = -(1.6f) * contSpeed / massSum;

                    Vector impulse = coll.norm;
                    vectorScale(&impulse, f);

                    Vector linear = impulse;
                    vectorScale(&linear, 1 / game.box.mass);
                    vectorAdd(&game.box.vel, &linear);

                    game.box.angV += vectorCross(&off, &impulse) / game.box.inertia;
                }

                // Fix position
                Vector fix = coll.norm;
                vectorScale(&fix, -coll.dist);
                vectorAdd(&game.box.pos, &fix);
            }
        }

        // reset forces
        vectorZero(&game.box.force);
        game.box.torq = 0;

        // frame cap
        int ticked = SDL_GetTicks() - startTick;
        if(SDL_TICKS_PASSED(ticked, 1000/FPS)) continue;
        SDL_Delay(1000/FPS - ticked);

        // render
        SDL_SetRenderDrawColor(rend, 50, 50, 50, 255);
        SDL_Rect rect = {0, 0, game.width, game.height};
        SDL_RenderFillRect(rend, &rect);

        boxRect.x = game.box.pos.x - game.box.width / 2;
        boxRect.y = game.box.pos.y - game.box.height / 2;
        SDL_RenderCopyEx(rend, game.box.tex,
                NULL, &boxRect,
                game.box.rot * 180 / M_PI,
                NULL, SDL_FLIP_NONE);

#ifdef DEBUG
        float minDist = fmax(game.width, game.height);
        Vector drawPos;
        for(Direction dir = UP; dir <= RIGHT; dir++) {
            Collision coll = collideBoxWall(&game.box.pos, game.box.width, game.box.height, game.box.rot, game.width, game.height, dir);
            if(coll.dist < minDist) {
                minDist = coll.dist;
                drawPos = coll.pos;
            }
        }
        SDL_SetRenderDrawColor(rend, 0, 0, 200, 255);
        SDL_RenderDrawLine(rend, drawPos.x - 5, drawPos.y - 5, drawPos.x + 5, drawPos.y + 5);
        SDL_RenderDrawLine(rend, drawPos.x + 5, drawPos.y - 5, drawPos.x - 5, drawPos.y + 5);
#endif

        SDL_SetRenderDrawColor(rend, 255, 255, 255, 255);
        for(int i = 0; i < MAX_FINGERS; i++) {
            Finger finger = game.fingers[i];
            if(finger.touch) {
                Vector off = game.box.off;
                vectorRotate(&off, game.box.rot);
                vectorAdd(&off, &game.box.pos);
                SDL_RenderDrawLine(rend, finger.pos.x, finger.pos.y, off.x, off.y);
            }
        }

        SDL_RenderPresent(rend);
    }

    // clean up
    for(int i = 0; i < jsticks_c; i++) {
        SDL_JoystickClose(jsticks[i]);
        jsticks[i] = NULL;
    }
    free(jsticks);

    SDL_DestroyTexture(game.box.tex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();

    return 0;
}

void applyForce(Box *box, Vector* pos, Vector* force) {
    Vector relPos = *pos;
    vectorSub(&relPos, &box->pos);

    float mag = vectorMag(force);
    float angle = vectorAngle(force) - (vectorAngle(&relPos));
    float torq = mag * sin(angle) * vectorMag(&relPos);
    if(fabsf(torq) < .00001f) torq = 0;

    box->torq += torq;
    vectorAdd(&box->force, force);
}

Finger *findFingerById(SDL_FingerID id, Finger *fingers, int fingers_c) {
    for(int i = 0; i < fingers_c; i++) {
        if(fingers[i].id == id) {
            return &fingers[i];
        }
    }
    return NULL;
}

Finger *findFreeFinger(Finger *fingers, int fingers_c) {
    for(int i = 0; i < fingers_c; i++) {
        if(!fingers[i].touch) {
            return &fingers[i];
        }
    }
    return NULL;
}
