#pragma once

#include <cstdint>
#include <functional>
#include <memory>

namespace RenderToy
{
    struct WindowDesc{
        const char* title = "RenderToy";
        int width = 1280;
        int height = 720;
        bool resizable = true;
    };

    struct WindowEvent{
        enum class Type{
            None,
            Quit,
            Resized
        };

        Type type = Type::None;
        int width = 0;
        int height = 0;
    };

    struct NativeWindowHandle{
    #if defined(_WIN32)
        void* hwnd = nullptr;
    #elif defined(__APPLE__)
        void* nsWindow = nullptr;
        void* metalLayer = nullptr;
    #endif
    };

    class Window{
    private:
        using WindowCallback = std::function<void(const WindowEvent&)>;

        struct Impl;
        std::unique_ptr<Impl> impl;

        int width = 0;
        int height = 0;
        bool shouldClose = false;

    public:
        Window();
        ~Window();

        bool init(const WindowDesc&);

        void pumpEvents(const WindowCallback& callback);

        inline auto getWidth() const{ return width; }
        inline auto getHeight() const{ return height; }
        inline auto getShouldClose() const{ return shouldClose; }

        NativeWindowHandle nativeHandle() const;
    };
}