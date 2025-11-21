#pragma once

#include "type_traits.hpp"

namespace RenderToy
{
    template<typename T>
    concept optional_type = is_std_optional_v<std::remove_cvref_t<T>>;
    template<typename T>
    concept pointer_type = std::is_pointer_v<std::remove_cvref_t<T>> ||
        std::is_null_pointer_v<std::remove_cvref_t<T>>;
    template<typename T>
    concept value_type = (!optional_type<T>) && (!pointer_type<T>);

    template <typename... Ts>
    concept all_pointer = (pointer_type<Ts> && ...);
    template <typename... Ts>
    concept all_optional = (optional_type<Ts> && ...);
    template <typename... Ts>
    concept all_value = (value_type<Ts> && ...);

    template<typename T>
    concept has_shutdown = requires(T t) {
        { t.shutdown() } -> std::same_as<void>; } &&
        std::is_trivially_destructible_v<T> &&
        std::is_destructible_v<T>;
}
