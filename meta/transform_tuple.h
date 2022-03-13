#pragma once

#include <tuple>
#include <type_traits>
#include <utility>

template <class F, class... Args>
using TransformResult = decltype(std::tuple<std::invoke_result_t<F, Args>...>());

template <class F, class... Types>
concept invocable_on_all_types = (std::is_invocable_v<F, Types> && ...);

template <class F, class Tuple, std::size_t... I>
constexpr auto TransformTupleHelper(F&& functor, const Tuple& args, std::index_sequence<I...>) {
    return std::tuple(functor(std::get<I>(args))...);
}

template <class F, class... Args>
constexpr auto TransformTuple(
    F&& functor, const std::tuple<Args...>& args) requires invocable_on_all_types<F, Args...> {
    return TransformTupleHelper(std::forward<F>(functor), args,
                                std::make_index_sequence<sizeof...(Args)>());
}

template <class F, size_t I = 0, class... Args>
constexpr auto TransformReduceTupleHelper(F&& functor, const std::tuple<Args...>& args) {
    if constexpr (I == sizeof...(Args)) {
        return std::tuple();
   } else if constexpr (std::is_void_v<decltype(functor(std::get<I>(args)))>) {
        return std::tuple_cat(TransformReduceTupleHelper<F, I + 1, Args...>(functor, args));
    } else {
        return std::tuple_cat(std::tuple(functor(std::get<I>(args))),
                              TransformReduceTupleHelper<F, I + 1, Args...>(functor, args));
    }
}

template <class F, class... Args>
constexpr auto TransformReduceTuple(
    F&& functor, const std::tuple<Args...>& args) requires invocable_on_all_types<F, Args...> {
    return TransformReduceTupleHelper(functor, args);
}
 
