#pragma once

#include "Log/ISink.hpp"

namespace RenderToy
{
    class ConsoleSink: public ISink{
    public:
        void write(const LogMessage&) override;
    };
}