#pragma once

#include <atomic>
#include <utility>
#include <optional>
#include <deque>
#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <iostream>

template <class T>
class BufferedChannel {
public:
    explicit BufferedChannel(size_t size) : size_(size) {
    }

    void Send(const T& value) {
        {
            std::unique_lock lock(mtx_);
            send_.wait(lock, [this] { return closed_.load() || buff_.size() < size_; });

            if (closed_.load()) {
                throw std::runtime_error("chan is closed");
            }

            buff_.emplace_back(value);
        }

        recv_.notify_one();
    }

    std::optional<T> Recv() {
        std::optional<T> ret;
        {
            std::unique_lock lock(mtx_);

            recv_.wait(lock, [this] { return closed_.load() || !buff_.empty(); });

            if (closed_.load()) {
                if (buff_.empty()) {
                    return std::nullopt;
                }

                ret.emplace(uff_.front());
                buff_.pop_front();

                return ret;
            }

            ret.emplace(buff_.front());
            buff_.pop_front();
        }

        send_.notify_one();

        return ret;
    }

    void Close() {
        closed_.store(true);
        send_.notify_all();
        recv_.notify_all();
    }

private:
    size_t size_;
    std::mutex mtx_;
    std::condition_variable send_;
    std::condition_variable recv_;
    std::deque<T> buff_;
    std::atomic_bool closed_{};
};
b
