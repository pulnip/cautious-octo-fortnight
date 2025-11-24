#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>
#include "core_types.hpp"

namespace RenderToy
{
    template<typename T>
    struct generic_handle{
        Index index = std::numeric_limits<Index>::max();
        std::uint32_t generation = 0;

        bool isValid() const{
            return index != std::numeric_limits<Index>::max();
        }

        friend bool operator==(const generic_handle& lhs, const generic_handle& rhs){
            return lhs.index == rhs.index && lhs.generation == rhs.generation;
        }
    };

    template<typename T>
    struct generic_handleHash{
        std::size_t operator()(const generic_handle<T>& handle) const {
            return std::hash<Index>()(handle.index) ^ (std::hash<std::uint32_t>()(handle.generation) << 1);
        }
    };
}