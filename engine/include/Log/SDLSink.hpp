#pragma once

#include "Log/ISink.hpp"

namespace RenderToy
{
    class SDLSink: public ISink{
    public:
        void write(const LogMessage&) override;
    };
}