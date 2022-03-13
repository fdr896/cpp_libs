#pragma once

#include <functional>
#include <boost/context/continuation.hpp>

class Coroutine {
public:
    inline static thread_local Coroutine* curr_co = nullptr;

public:
    explicit Coroutine(std::function<void()> f) {
        ctx_ = boost::context::callcc([f = std::move(f), this](boost::context::continuation&& c) {
            suspend_ = [&c] { c = c.resume(); };
            suspend_();
            f();
            return std::move(c);
        });

        curr_co = this;
        ctx_ = ctx_.resume();
    }

    void resume() {
        if (curr_co == this) {
            throw std::runtime_error("resume on oneself is not allowed");
        }

        curr_co = this;
        if (!ctx_) {
            curr_co = nullptr;
            throw std::runtime_error("resume on lasted coroutine");
        }
        ctx_ = ctx_.resume();
    }

    void suspend() {
        curr_co = nullptr;
        suspend_();
    }

    ~Coroutine() {
        curr_co = nullptr;
    }

private:
    boost::context::continuaton ctx_;
    std::function<void()> suspend_;
};

void suspend() {
    if (Coroutine::curr_co == nullptr) {
        throw std::runtime_error("suspend without coroutine");
    }

    Coroutine::curr_co->suspend();
}i
