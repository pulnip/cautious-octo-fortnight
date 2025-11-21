#pragma once

#include <algorithm>
#include <string>

namespace RenderToy
{
    inline std::string toUpper(const std::string& text){
        std::string upper = text;
        std::transform(text.begin(), text.end(), upper.begin(),
            [](unsigned char c){ return std::toupper(c); }
        );
        return upper;
    }
}