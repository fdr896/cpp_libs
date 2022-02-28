#pragma once

#include <exception>
#include <type_traits>

class BadWeakPtr : public std::exception {};

template <typename T>
class SharedPtr;
template <typename T>
class WeakPtr;
class EnableSharedFromThisBase;
template <typename T>
class EnableSharedFromThis;

struct ControlBlockBase {
    virtual ~ControlBlockBase() = default;
    size_t ref_count_{};
    size_t weak_count_{};
};
template <typename U>
struct ControlBlock : ControlBlockBase {
    ControlBlock(U* ptr) : ptr_(ptr) {
    }

    ~ControlBlock() override {
        if (ref_count_ == 0) {
            delete ptr_;
        }
    }

    U* ptr_ = nullptr;
};
template <typename U>
struct ControlBlockInplace : ControlBlockBase {
    template <typename... Args>
    ControlBlockInplace(Args&&... args) {
        new (&storage_) U(std::forward<Args>(args)...);
    }

    U* Get() {
        return reinterpret_cast<U*>(&storage_);
    }

    ~ControlBlockInplace() {
        if (ref_count_ == 0) {
            reinterpret_cast<U*>(&storage_)->~U();
        }
    }

    std::aligned_storage_t<sizeof(U), alignof(U)> storage_;
};
