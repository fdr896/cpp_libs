#pragma once

#include <cstddef>  // std::nullptr_t
#include <memory>
#include <type_traits>

#include "sw_fwd.h"  // Forwad declaration

class EnableSharedFromThisBase {};

template <typename T>
class EnableSharedFromThis : public EnableSharedFromThisBase {
    template <typename U>
    friend class SharedPtr;

public:
    SharedPtr<T> SharedFromThis() {
        return self_.Lock();
    }

    SharedPtr<const T> SharedFromThis() const {
        return self_;
    }

    WeakPtr<T> WeakFromThis() noexcept {
        return self_;
    }

    WeakPtr<const T> WeakFromThis() const noexcept {
        return self_;
    }

private:
    WeakPtr<T> self_;
};

// https://en.cppreference.com/w/cpp/memory/shared_ptr
template <typename T>
class SharedPtr {
    template <typename U, typename... Args>
    friend SharedPtr<U> MakeShared(Args&&... args);

public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    SharedPtr() = default;
    SharedPtr(std::nullptr_t) {
        ptr_ = nullptr;
        cb_ = nullptr;
    }
    template <typename U>
    explicit SharedPtr(U* ptr) : ptr_(ptr), cb_(new ControlBlock<U>(static_cast<U*>(ptr_))) {
        cb_->ref_count_ = 1;

        if constexpr (std::is_convertible_v<U, EnableSharedFromThisBase>) {
            LinkShared(ptr);
        } else {
        }
    }

    template <typename U>
    SharedPtr(const SharedPtr<U>& other) : ptr_(other.Get()), cb_(other.GetCB()) {
        Increment();
    }
    SharedPtr(const SharedPtr& other) : ptr_(other.Get()), cb_(other.GetCB()) {
        Increment();
    }
    template <typename U>
    SharedPtr(SharedPtr<U>&& other) : ptr_(other.Get()), cb_(other.GetCB()) {
        other.Nullify();
    }
    SharedPtr(SharedPtr&& other) {
        ptr_ = other.Get();
        cb_ = other.GetCB();
        other.Nullify();
    }

    // Aliasing constructor
    // #8 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    template <typename Y>
    SharedPtr(const SharedPtr<Y>& other, T* ptr) : ptr_(ptr), cb_(other.GetCB()) {
        Increment();
    }

    // Promote `WeakPtr`
    // #11 from https://en.cppreference.com/w/cpp/memory/shared_ptr/shared_ptr
    explicit SharedPtr(const WeakPtr<T>& other) {
        if (other.Expired()) {
            throw BadWeakPtr{};
        }

        ptr_ = other.Get();
        cb_ = other.GetCB();
        Increment();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    template <typename U>
    SharedPtr& operator=(const SharedPtr<U>& other) {
        if (*this == other) {
            return *this;
        }

        Unlink();

        ptr_ = other.Get();
        cb_ = other.GetCB();
        Increment();

        return *this;
    }
    SharedPtr& operator=(const SharedPtr& other) {
        if (this == &other) {
            return *this;
        }

        Unlink();

        ptr_ = other.Get();
        cb_ = other.GetCB();
        Increment();

        return *this;
    }
    template <typename U>
    SharedPtr& operator=(SharedPtr<U>&& other) {
        if (*this == other) {
            return *this;
        }

        Unlink();

        ptr_ = other.Get();
        cb_ = other.GetCB();
        other.Nullify();

        return *this;
    }
    SharedPtr& operator=(SharedPtr&& other) {
        if (this == &other) {
            return *this;
        }

        Unlink();

        ptr_ = other.Get();
        cb_ = other.GetCB();
        other.Nullify();

        return *this;
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~SharedPtr() {
        Reset();
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    void Reset() {
        Decrement();
        DistructLast();
    }
    template <typename U>
    void Reset(U* ptr) {
        if (ptr_ == ptr) {
            return;
        }
        Reset();

        ptr_ = ptr;
        cb_ = new ControlBlock<U>(ptr);
        Increment();
    }
    void Swap(SharedPtr& other) {
        std::swap(ptr_, other.ptr_);
        std::swap(cb_, other.cb_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_;
    }
    T& operator*() const {
        return *ptr_;
    }
    T* operator->() const {
        return ptr_;
    }
    size_t UseCount() const {
        if (cb_ == nullptr) {
            return 0;
        }
        return cb_->ref_count_;
    }
    explicit operator bool() const {
        return ptr_ != nullptr;
    }

    ControlBlockBase* GetCB() const {
        return cb_;
    }
    void Nullify() {
        ptr_ = nullptr;
        cb_ = nullptr;
    }

private:
    void DistructLast() {
        if (cb_ != nullptr && cb_->ref_count_ == 0) {
            if (cb_->weak_count_ == 0) {
                delete cb_;
            } else {
                ptr_->~T();
            }
            cb_ = nullptr;
            ptr_ = nullptr;
        }
    }
    void Increment() {
        if (cb_ != nullptr) {
            cb_->ref_count_ += 1;
        }
    }
    void Decrement() {
        if (cb_ != nullptr) {
            cb_->ref_count_ -= 1;
        }
    }
    void Unlink() {
        Decrement();
        if (cb_ != nullptr && cb_->ref_count_ == 0) {
            if (cb_->weak_count_ == 0) {
                delete cb_;
            } else {
                ptr_->~T();
            }
            cb_ = nullptr;
            ptr_ = nullptr;
        }
    }

    template <typename U>
    void LinkShared(EnableSharedFromThis<U>* ptr) {
        ptr->self_ = *this;
    }

    T* ptr_ = nullptr;
    ControlBlockBase* cb_ = nullptr;
};

template <typename T, typename U>
inline bool operator==(const SharedPtr<T>& left, const SharedPtr<U>& right) {
    return (left.Get() == right.Get()) && (left.GetCB() == right.GetCB());
}

template <typename U, typename... Args>
SharedPtr<U> MakeShared(Args&&... args) {
    SharedPtr<U> sp;
    ControlBlockInplace<U>* cb = new ControlBlockInplace<U>(std::forward<Args>(args)...);
    sp.cb_ = cb;
    sp.ptr_ = cb->Get();
    sp.cb_->ref_count_ = 1;
    if constexpr (std::is_convertible_v<U, EnableSharedFromThisBase>) {
        sp.LinkShared(sp.ptr_);
    } else {
    }

    return sp;
}
