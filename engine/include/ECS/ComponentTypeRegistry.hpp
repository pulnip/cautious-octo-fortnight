#pragma once

#include <typeindex>
#include <unordered_map>
#include "Component.hpp"

namespace RenderToy
{
    class ComponentTypeRegistry{
    private:
        std::unordered_map<std::type_index, uint32_t> componentOrderMap;


    };
}