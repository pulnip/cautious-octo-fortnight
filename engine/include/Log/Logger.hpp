#pragma once

#include <memory>
#include <mutex>
#include <vector>
#include "Log/ISink.hpp"

namespace RenderToy
{
    class Logger{
        using SinkPtr = std::unique_ptr<ISink>;

    public:
        static Logger& instance();

        void addSink(SinkPtr sink);

        void setMinLevel(LogLevel level){
            minLevel = level;
        }
        LogLevel getMinLevel() const{
            return minLevel;
        }

        void log(LogMessage&& msg);

    private:
        Logger() = default;

        std::vector<SinkPtr> sinks;
        LogLevel minLevel = LogLevel::Debug;
        std::mutex mtx;
    };
}