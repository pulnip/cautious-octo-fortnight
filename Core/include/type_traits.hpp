#pragma

#include <optional>

namespace RenderToy
{
    template<typename T>
    struct fn_traits;

    template<typename R, typename... Args>
    struct fn_traits<R (*)(Args...)>{ using args = std::tuple<Args...>; };

    template<typename C, typename R, typename... Args>
    struct fn_traits<R (C::*)(Args...) const>{ using args = std::tuple<Args...>; };

    template<typename F>
    using fn_args_t = typename fn_traits<decltype(&std::decay_t<F>::operator())>::args;

    template<typename Tuple>
    struct decay_tuple;
    template<typename... Ts>
    struct decay_tuple<std::tuple<Ts...>>{ using type = std::tuple<std::decay_t<Ts>...>; };

    template<typename F>
    using fn_decayed_args_t = typename decay_tuple<fn_args_t<F>>::type;

    template<typename T> struct is_std_optional: std::false_type{};
    template<typename U> struct is_std_optional<std::optional<U>>: std::true_type{};

    template<typename T>
    inline constexpr bool is_std_optional_v = is_std_optional<std::remove_cv_t<T>>::value;

    template<typename T>
    struct remove_optional{ using type = T; };
    template<typename T>
    struct remove_optional<std::optional<T>>{ using type = T; };

    template<typename T>
    using remove_optional_t = typename remove_optional<T>::type;

    static_assert(std::same_as<int, remove_optional_t<std::optional<int>>>);
    static_assert(std::same_as<int, remove_optional_t<int>>);
}
