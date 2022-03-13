#pragma once

#include <vector>
#include <utility>
#include <atomic>

template <class T>
class MPMCBoundedQueue {
public:
    explicit MPMCBoundedQueue(size_t size) : max_size_(size), q_(size) {
        for (size_t i = 0; i < size; ++i) {
            q_[i].gen_.store(i);
        }
    }

    bool Enqueue(const T& value) {
        if (t_.load() - h_.load() == max_size_) {
            return false;
        }

        while (true) {
            size_t expected = t_.load();
            if (q_[t_.load() & (max_size_ - 1)].gen_ < expected) {
                return false;
            }

            if (!t_.compare_exchange_weak(expected, expected + 1)) {
                continue;
            }

            expected &= (max_size_ - 1);
            q_[expected].val_ = value;
            q_[expected].gen_.fetch_add(1);

            return true;
        }
    }

    bool Dequeue(T& data) {
        if (t_.load() == h_.load()) {
            return false;
        }

        while (true) {
            size_t expected = h_.load();
            if (q_[h_ & (max_size_ - 1)].gen_ < expected + 1) {
                return false;
            }

            if (!h_.compare_exchange_weak(expected, expected + 1)) {
                continue;
            }

            auto old = expected;
            expected &= (max_size_ - 1);
            data = std::move(q_[expected].val_);
            q_[expected].gen_ = old + max_size_;

            return true;
        }
    }

private:
    struct Node {
        Node() = default;

        T val_{};
        std::atomic<size_t> gen_{};
    };

    size_t max_size_;
    std::atomic<size_t> sz_{};
    std::atomic<size_t> h_{};
    std::atomic<size_t> t_{};
    std::vector<Node> q_;
};
