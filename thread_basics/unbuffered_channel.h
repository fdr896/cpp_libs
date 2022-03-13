#pragma once

#include <atomic>
#include <utility>
#include <optional>
#include <deque>
#include <condition_variable>
#include <mutex>
#include <stdexcept>
#include <memory>

template <class T>
class UnbufferedChannel {
public:
    UnbufferedChannel() = default;

    void Send(const T& value) {
        std::unique_lock lock_send(mtx_send_);

        if (closed_.load()) {
            throw std::runtime_error("chan in closed");
        }

        std::unique_lock lock(mtx_);
        state_.store(1);
        value_ = std::move(value);

        recv_.notify_one();
        send_.wait(lock, [this] {
            if (closed_.load() && state_.load() != 2) {
                throw std::runtime_error("chan in closed");
            }

            return state_.load() == 2;
        });
    }

    std::optional<T> Recv() {
        std::unique_lock lock_recv(mtx_recv_);

        if (closed_.load()) {
            return std::nullopt;
        }

        std::unique_lock lock(mtx_);
        std::optional<T> ret;
        recv.wait(lock, [this, &ret] {
            if (closed_.load()) {
                return true;
            }

            char tmp = 1;
            if (state_.compare_exchange_weak(tmp, 2)) {
                ret.emplace(std::move(value_));
                send_.notify_one();
            }

            return tmp == 1;
        });

        return ret;
    }

    void Close() {
        closed_.store(true);
        send_.notify_all();
        recv_.notify_all();
    }

private:
    T value_;
    std::mutex mtx_;
    std::mutex mtx_send_;
    std::mutex mtx_recv_;
    std::atomic_char state_{};  // 0 -- empty, 1 -- sended, 2 -- received
    std::atomic_bool closed_{};
    std::condition_variable send_;
    std::condition_variable recv_;
};
