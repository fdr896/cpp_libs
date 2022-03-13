#pragma once

#include <functional>
#include <utility>
#include <memory>
#include <tuple>

template <class F, class... Args>
constexpr auto BindFront(F&& f, Args&&... args) {
    return [ f = std::move(f), ... args = std::forward<Args>(args) ]<class... RemainArgs>(
        RemainArgs && ... others) mutable {
        return f(std::forward<decltype(args)>(args)..., std::forward<RemainArgs>(others)...);
    };
}
