#pragma once

#include <cstddef>
#include <cstdint>
#include <limits>
#include <utility>

namespace RenderToy
{
    struct Handle{
        Index index;
        std::uint32_t generation;
    };
    constexpr auto invalidHandle(){
        return Handle{
            .index = std::numeric_limits<Index>::max(),
            .generation = std::numeric_limits<std::uint32_t>::max()};
    }
    constexpr auto operator==(Handle lhs, Handle rhs){
        return lhs.index==rhs.index && lhs.generation==rhs.generation;
    }
    constexpr auto operator!=(Handle lhs, Handle rhs){
        return !(lhs==rhs);
    }
    constexpr auto operator<(const Handle lhs, const Handle rhs){
        return lhs.index<rhs.index;
    }
    struct HandleHash{
        std::size_t operator()(const Handle& handle) const {
            return std::hash<Index>()(handle.index) ^ (std::hash<std::uint32_t>()(handle.generation) << 1);
        }
    };
}