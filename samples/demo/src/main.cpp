#include <cstdio>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include "engine.hpp"
#include "demo.hpp"

using RenderToy::Engine;

SDL_AppResult SDL_AppInit([[maybe_unused]] void** appState,
    [[maybe_unused]] int argc, [[maybe_unused]] char* argv[]
){
    if(!SDL_SetAppMetadata("RenderToy", "0.0.1", "com.example.rendertoy")){
        return SDL_APP_FAILURE;
    }

    auto window = SDL_CreateWindow("RenderToy", 800, 600, 0);
    if(!window){
        return SDL_APP_FAILURE;
    }

    auto engine = new Engine();
    engine->start();

    *appState = engine;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appState){
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent([[maybe_unused]] void* appState,
    SDL_Event* event
){
    switch(event->type){
    case SDL_EVENT_QUIT:
    case SDL_EVENT_KEY_DOWN:
        switch(event->key.scancode){
        case SDL_SCANCODE_ESCAPE:
            return SDL_APP_SUCCESS;
        default:
            break;
        }
        break;
    default:
        break;
    }

    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appState, [[maybe_unused]] SDL_AppResult result){
    switch(result){
    case SDL_APP_SUCCESS:
        fprintf(stdout, "SDL_APP_SUCCESS\n");
        break;
    case SDL_APP_FAILURE:
        fprintf(stdout, "SDL_APP_FAILURE\n");
        break;
    default:
        break;
    }

    delete static_cast<Engine*>(appState);
}
