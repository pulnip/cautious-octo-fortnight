#include <print>
#include "Engine.hpp"

#include "Log/Log.hpp"
#include "Log/ConsoleSink.hpp"
#include "Log/SDLSink.hpp"

namespace RenderToy
{
    void Engine::start(){
        Logger::instance().addSink(std::make_unique<ConsoleSink>());

        LOG_ERROR(LOG_CORE, "Engine Start");
    }
}