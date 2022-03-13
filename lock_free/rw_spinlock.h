#pragma once

#include <atomic>

class RWSpinLock {
    static const uint32_t kDeleteOne = static_cast<uint32_t>(-2);

public:
    void LockRead() {
        while (counter_.load() % 2 != 0) {
        }
        counter_.fetch_add(2);
    }

    void UnlockRead() {
        counter_.fetch_sub(2);
    }

    void LockWrite() {
        uint32_t expected = 0;
        while (!counter_.compare_exchange_weak(expected, 1)) {
            expected = 0;
        }
    }

    void UnlockWrite() {
        counter_.store(0);
    }

private:
    std::atomic<uint32_t> counter_{};
};
