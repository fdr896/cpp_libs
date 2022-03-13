#pragma once

#include <atomic>
#include <optional>
#include <stdexcept>
#include <memory>
#include <utility>

template <class T>
class MPSCStack {
    struct Node {
        T value_;
        Node* next_ = nullptr;
    };

public:
    // Push adds one element to stack top.
    //
    // Safe to call from multiple threads.
    void Push(const T& value) {
        auto new_head = new Node{value, head_.load()};

        while (!head_.compare_exchange_weak(new_head->next_, new_head)) {
        }
    }

    // Pop removes top element from the stack.
    //
    // Not safe to call concurrently.
    std::optional<T> Pop() {
        auto old_head = head_.load();
        if (!old_head) {
            return std::nullopt;
        }

        std::optional<T> value;
        while (!head_.compare_exchange_weak(old_head, old_head->next_)) {
        }
        value.emplace(std::move(old_head->value_));
        delete old_head;

        return value;
    }

    // DequeuedAll Pop's all elements from the stack and calls cb() for each.
    //
    // Not safe to call concurrently with Pop()
    template <class TFn>
    void DequeueAll(const TFn& cb) {
        while (head_.load()) {
            cb(Pop().value());
        }
    }

    ~MPSCStack() {
        while (head_.load()) {
            auto del = head_.load();
            head_ = del->next_;
            delete del;
        }
    }

private:
    std::atomic<Node*> head_{};
};
