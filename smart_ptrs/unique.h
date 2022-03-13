#pragma once

#include "compressed_pair.h"

#include <cstddef>  // std::nullptr_t
#include <utility>
#include <memory>

// Primary template
template <typename T, typename Deleter = std::default_delete<T>>
class UniquePtr {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : ptr_(ptr, Deleter()) {
    }
    template <typename Deleter1>
    UniquePtr(T* ptr, Deleter1&& deleter) : ptr_(ptr, std::forward<Deleter1>(deleter)) {
    }

    template <typename T1, typename Deleter1>
    UniquePtr(UniquePtr<T1, Deleter1>&& other) noexcept
        : ptr_(static_cast<T*>(other.Release()), std::move(other.GetDeleter())) {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    UniquePtr& operator=(UniquePtr&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        GetDeleter()(ptr_.GetFirst());
        ptr_.GetFirst() = other.Get();
        other.ptr_.GetFirst() = nullptr;
        ptr_.GetSecond() = std::forward<Deleter>(other.GetDeleter());

        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) noexcept {
        GetDeleter()(ptr_.GetFirst());
        ptr_.GetFirst() = nullptr;

        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() noexcept {
        GetDeleter()(ptr_.GetFirst());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* old = ptr_.GetFirst();
        ptr_.GetFirst() = nullptr;

        return old;
    }
    void Reset(T* ptr = nullptr) {
        T* old = ptr_.GetFirst();
        ptr_.GetFirst() = ptr;
        GetDeleter()(old);
    }
    void Swap(UniquePtr& other) {
        std::swap(ptr_, other.ptr_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_.GetFirst();
    }
    Deleter& GetDeleter() {
        return ptr_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return ptr_.GetSecond();
    }
    explicit operator bool() const {
        return (ptr_.GetFirst() != nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    std::add_lvalue_reference_t<T> operator*() const {
        return *ptr_.GetFirst();
    }
    T* operator->() const {
        return ptr_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> ptr_;
};

// Array specialization
template <typename T, typename Deleter>
class UniquePtr<T[], Deleter> {
public:
    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Constructors

    explicit UniquePtr(T* ptr = nullptr) : ptr_(ptr, Deleter()) {
    }
    template <typename Deleter1>
    UniquePtr(T* ptr, Deleter1&& deleter) : ptr_(ptr, std::forward<Deleter1>(deleter)) {
    }

    template <typename T1, typename Deleter1>
    UniquePtr(UniquePtr<T1, Deleter1>&& other) noexcept
        : ptr_(static_cast<T*>(other.Release()), std::move(other.GetDeleter())) {
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // `operator=`-s

    template <typename T1, typename Deleter1>
    UniquePtr& operator=(UniquePtr<T1, Deleter1>&& other) noexcept {
        if (this == &other) {
            return *this;
        }
        GetDeleter()(ptr_.GetFirst());
        ptr_.GetFirst() = other.Get();
        other.ptr_.GetFirst() = nullptr;
        ptr_.GetSecond() = std::forward<Deleter>(other.GetDeleter());

        return *this;
    }
    UniquePtr& operator=(std::nullptr_t) noexcept {
        GetDeleter()(ptr_.GetFirst());
        ptr_.GetFirst() = nullptr;

        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Destructor

    ~UniquePtr() noexcept {
        GetDeleter()(ptr_.GetFirst());
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Modifiers

    T* Release() {
        T* old = ptr_.GetFirst();
        ptr_.GetFirst() = nullptr;

        return old;
    }
    void Reset(T* ptr = nullptr) {
        T* old = ptr_.GetFirst();
        ptr_.GetFirst() = ptr;
        GetDeleter()(old);
    }
    void Swap(UniquePtr& other) {
        std::swap(ptr_, other.ptr_);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Observers

    T* Get() const {
        return ptr_.GetFirst();
    }
    T& operator[](size_t i) {
        return ptr_.GetFirst()[i];
    }
    Deleter& GetDeleter() {
        return ptr_.GetSecond();
    }
    const Deleter& GetDeleter() const {
        return ptr_.GetSecond();
    }
    explicit operator bool() const {
        return (ptr_.GetFirst() != nullptr);
    }

    ////////////////////////////////////////////////////////////////////////////////////////////////
    // Single-object dereference operators

    T& operator*() const {
        return *ptr_.GetFirst();
    }
    T* operator->() const {
        return ptr_.GetFirst();
    }

private:
    CompressedPair<T*, Deleter> ptr_;
