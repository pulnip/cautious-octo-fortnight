#include <cstdio>
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL_main.h>
#include "Engine.hpp"
#include "Demo.hpp"

using RenderToy::Engine, Demo::DemoGame;
using RenderToy::LOG_CORE;

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
    engine->onInit(new DemoGame());

    *appState = engine;

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appState){
    auto engine = static_cast<Engine*>(appState);

    engine->onUpdate(1/60.0f);

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

void SDL_AppQuit(void* appState, SDL_AppResult result){
    auto engine = static_cast<Engine*>(appState);

    switch(result){
    case SDL_APP_SUCCESS:
        LOG_DEBUG(LOG_CORE, "SDL_APP_SUCCESS");
        break;
    case SDL_APP_FAILURE:
        LOG_DEBUG(LOG_CORE, "SDL_APP_FAILURE");
        break;
    default:
        break;
    }

    engine->onShutdown();

    delete engine;
}
