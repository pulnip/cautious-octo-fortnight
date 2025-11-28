#include "Platform/Window.hpp"
#include <cassert>
#include <SDL3/SDL.h>
#include "Log/Log.hpp"

namespace RenderToy
{
    struct Window::Impl{
        SDL_Window* window = nullptr;
    #if defined(__APPLE__)
        SDL_MetalView view;
    #endif
    };

    Window::Window():impl(std::make_unique<Impl>()){}

    Window::~Window(){
        if(impl){
        #if defined(__APPLE__)
            if(impl->view)
                SDL_Metal_DestroyView(impl->view);
        #endif
            if(impl->window)
                SDL_DestroyWindow(impl->window);

            SDL_QuitSubSystem(SDL_INIT_VIDEO);
            SDL_Quit();
        }
    }

    bool Window::init(const WindowDesc& desc){
        if(!SDL_SetAppMetadata("RenderToy", "0.0.1", "com.example.rendertoy")){
            return false;
        }

        if(!SDL_Init(SDL_INIT_VIDEO)){
            LOG_ERROR(LOG_CORE, "SDL_Init failed: {}", SDL_GetError());
            return false;
        }

        Uint32 flags = 0;
        if(desc.resizable)
            flags |= SDL_WINDOW_RESIZABLE;

        auto window = SDL_CreateWindow(desc.title,
            desc.width, desc.height, flags);

        if(!window){
            LOG_ERROR(LOG_CORE, "SDL_CreateWindow failed: {}", SDL_GetError());
            return false;
        }

        this->width = desc.width;
        this->height = desc.height;
        this->shouldClose = false;

        this->impl->window = window;

    #if defined(__APPLE__)
        // Create Metal view for rendering
        impl->view = SDL_Metal_CreateView(window);
        if (!impl->view) {
            LOG_ERROR(LOG_CORE, "Failed to create Metal view: {}", SDL_GetError());
            SDL_DestroyWindow(window);
            return false;
        }
    #endif

        return true;
    }

    void Window::pumpEvents(const WindowCallback& callback){
        SDL_Event event;
        while(SDL_PollEvent(&event)){
            WindowEvent windowEvent;

            switch(event.type){
            case SDL_EVENT_QUIT:
                windowEvent.type = WindowEvent::Type::Quit;
                shouldClose = true;
                callback(windowEvent);
                break;
            case SDL_EVENT_WINDOW_RESIZED:
                if(event.window.windowID == SDL_GetWindowID(impl->window)){
                    windowEvent.type = WindowEvent::Type::Resized;
                    this->width = windowEvent.width = event.window.data1;
                    this->height = windowEvent.height = event.window.data2;
                    callback(windowEvent);
                }
                break;
            }
        }
    }

    NativeWindowHandle Window::nativeHandle() const{
        assert(impl && impl->window);
        auto props = SDL_GetWindowProperties(impl->window);

    #if defined(_WIN32)
        return NativeWindowHandle{
            .hwnd = SDL_GetPointerProperty(props,
                SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL);
        };
    #elif defined(__APPLE__)
        return NativeWindowHandle{
            .nsWindow = SDL_GetPointerProperty(props,
                SDL_PROP_WINDOW_COCOA_WINDOW_POINTER, NULL),
            .metalLayer = SDL_Metal_GetLayer(impl->view),
        };
    #endif
    }
}