/*
 * Author: Rio
 * Date: 2017/05/21
 */

#include <math.h>

#include "SDL2/SDL.h"
#include "game.h"
#include "vector.h"

int main(int argc, char *argv[]) {
    // Log everything
    SDL_LogSetAllPriority(SDL_LOG_PRIORITY_INFO);

    Game game = {
        .width = 600,
        .height = 900,
        .running = 1,
        .box = {
            .width = 80,
            .height = 20,
            .pos = {game.width / 2, game.height / 2},
            .off = {35, 0},
            .mass = 1
        },
        .gForce = .2,
        .fForce = 1
    };

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) != 0) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to init: %s", SDL_GetError());
        return 1;
    }

    SDL_Window *win = SDL_CreateWindow("Box",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            game.width, game.height, SDL_WINDOW_SHOWN);
    if(!win) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create window: %s", SDL_GetError());
        return 1;
    }

    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    if(!rend) {
        SDL_LogError(SDL_LOG_CATEGORY_ERROR, "Failed to create renderer: %s", SDL_GetError());
        return 1;
    }

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

    // configure SDL
    SDL_SetHint(SDL_HINT_ACCELEROMETER_AS_JOYSTICK, "1");
    SDL_SetHint(SDL_HINT_VIDEO_ALLOW_SCREENSAVER, "1");

    // Box texture
    game.box.tex = SDL_CreateTexture(rend,
            SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
            game.box.width, game.box.height);
    SDL_SetRenderTarget(rend, game.box.tex);
    SDL_SetRenderDrawColor(rend, 255, 221, 0, 255);
    SDL_Rect boxRect = {0, 0, game.box.width, game.box.height};
    SDL_RenderFillRect(rend, &boxRect);
    SDL_SetRenderDrawColor(rend, 255, 0, 0, 255);
    SDL_Point off = {game.box.off.x + game.box.width / 2, game.box.off.y + game.box.height / 2};
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
                    }
                    break;
                case SDL_JOYAXISMOTION:
                    {
                        int value = eve.jaxis.value / 32767;
                        if(eve.jaxis.axis == 0) {
                            // x axis
                            game.grav.x += value * game.gForce;
                        } else {
                            // y axis
                            game.grav.y += value * game.gForce;
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
                        SDL_Log("Finger id %d", id);
                        if(id > MAX_FINGERS) break;
                        game.fingers[id].pos.x = eve.tfinger.x * game.width;
                        game.fingers[id].pos.y = eve.tfinger.y * game.height;
                        game.fingers[id].touch = 1;
                    }
                    break;
                case SDL_FINGERUP:
                    game.fingers[eve.tfinger.fingerId].touch = 0;
                    break;
                case SDL_QUIT:
                    game.running = 0;
                    break;
            }
        }


        // game logic
        // reset acceleration
        vectorZero(&game.box.accl);
        game.box.torq = 0;

        // apply gravity
        vectorAdd(&game.box.accl, &game.grav);

        // apply other forces
        for(int i = 0; i < MAX_FINGERS; i++) {
            Finger finger = game.fingers[i];
            if(finger.touch) {

                Vector off = game.box.off;
                vectorRotate(&off, game.box.rot);
                vectorAdd(&off, &game.box.pos);

                Vector force = {(finger.pos.x - off.x) / game.width, (finger.pos.y - off.y) / game.width};
                vectorScale(&force, game.fForce);

                float mag = vectorMag(&force);
                float angle = vectorAngle(&force) - (vectorAngle(&game.box.off) + game.box.rot);
                float torq = mag * sin(angle) * vectorMag(&game.box.off);

                game.box.torq += torq / (game.box.mass * (game.box.width * game.box.width + game.box.height * game.box.height) / 12);

                vectorAdd(&game.box.accl, &force);
            }
        }

        vectorAdd(&game.box.vel, &game.box.accl);
        vectorAdd(&game.box.pos, &game.box.vel);

        game.box.rotV += game.box.torq;
        game.box.rot += game.box.rotV;

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
                game.box.rot * 180 / 3.1415927,
                NULL, SDL_FLIP_NONE);

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
