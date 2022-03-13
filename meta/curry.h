#pragma once

#include <utility>
#include <memory>

template <class F>
constexpr auto Curry(F&& f) {
    return [f = std::move(f)](auto&&... args) mutable {
        return f(std::make_tuple(std::forward<decltype(args)>(args)...));
    };
}

template <class F>
constexpr auto Uncurry(F&& f) {
    return [f = std::move(f)]<class Tup>(Tup&& tup) mutable { return std::apply(f, tup); };
}
