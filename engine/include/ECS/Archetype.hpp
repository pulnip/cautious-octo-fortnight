#pragma once

#include <cstdint>

namespace RenderToy
{
    using ArchetypeBit = std::uint64_t;

    constexpr bool isSubset(ArchetypeBit lhs, ArchetypeBit rhs){
        return (lhs & rhs) == lhs;
    }
}