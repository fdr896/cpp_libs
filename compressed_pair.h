#pragma once

#include <iostream>
#include <type_traits>

template <typename T, bool Diff, bool = std::is_empty_v<T> && !std::is_final_v<T>>
class Element {
public:
    Element() = default;
    template <typename U>
    Element(U&& value) : value_(std::forward<U>(value)) {
    }

    T& GetValue() {
        return value_;
    }
    const T& GetValue() const {
        return value_;
    }

private:
    T value_;
};

template <typename T, bool Diff>
class Element<T, Diff, true> : public T {
public:
    Element() = default;
    template <typename U>
    Element(U&&) {
    }

    T& GetValue() {
        return *this;
    }
    const T& GetValue() const {
        return *this;
    }
};

template <class T, class S>
class CompressedPair : private Element<T, false>, private Element<S, true> {
    using First = Element<T, false>;
    using Second = Element<S, true>;

public:
    CompressedPair()
        : First(),
          Second(){

          };
    template <class T1, class S1>
    CompressedPair(T1&& first, S1&& second)
        : First(std::forward<T1>(first)), Second(std::forward<S1>(second)) {
    }

    T& GetFirst() {
        return First::GetValue();
    }
    const T& GetFirst() const {
        return First::GetValue();
    }

    S& GetSecond() {
        return Second::GetValue();
    }
    const S& GetSecond() const {
        return Second::GetValue();
    }
};
