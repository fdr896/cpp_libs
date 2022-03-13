#pragma once

#include <mutex>
#include <condition_variable>
#include <thread>
#include <deque>
#include <map>

class DefaultCallback {
public:
    void operator()(int& value) {
        --value;
    }
};

class Semaphore {
public:
    Semaphore(int count) : count_(count) {
    }

    void Leave() {
        std::unique_lock lock(mutex_);
        ++count_;
        cv_.notify_one();
    }

    template <class Func>
    void Enter(Func callback) {
        std::unique_lock lock(mutex_);

        if (count_ == 0) {
            size_t id = max_id_++;
            threads_.push_back(id);
            while (count_ == 0 && threads_.front() != id) {
                cv_.wait(lock);
            }
            threads_.pop_front();
        }

        callback(count_);
    }

    void Enter() {
        DefaultCallback callback;
        Enter(callback);
    }

private:
    std::mutex mutex_;
    std::condition_variable cv_;
    int count_ = 0;
    size_t max_id_ = 0;
    std::deque<size_t> threads_{};
};
