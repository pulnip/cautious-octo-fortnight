#pragma once

namespace RenderToy
{
    enum LogLevel{
        Trace,
        Debug,
        Info,
        Warn,
        Error,
        Fatal
    };

    inline const char* levelToString(LogLevel level){
        switch(level){
        case LogLevel::Trace: return "Trace";
        case LogLevel::Debug: return "Debug";
        case LogLevel::Info:  return "Info";
        case LogLevel::Warn:  return "Warn";
        case LogLevel::Error: return "Error";
        case LogLevel::Fatal: return "Fatal";
        default:              return "Unknown";
        }
    }
}