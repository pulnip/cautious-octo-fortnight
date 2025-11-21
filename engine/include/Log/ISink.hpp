#pragma once

#include "Log/Message.hpp"

namespace RenderToy
{
    class ISink{
    public:
        virtual ~ISink() = default;
        virtual void write(const LogMessage&) = 0;
    };
}