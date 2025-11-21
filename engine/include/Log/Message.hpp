#pragma once

#include <chrono>
#include <string>
#include <source_location>
#include <thread>
#include "Log/Category.hpp"
#include "Log/Level.hpp"

namespace RenderToy
{
    struct LogMessage{
        LogLevel level;
        LogCategory category;
        std::string text;

        std::source_location location;

        std::thread::id thread_id;
        std::chrono::system_clock::time_point time_point;
    };
}