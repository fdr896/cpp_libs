#pragma once

#include <functional>
#include <memory>
#include <boost/context/continuation.hpp>

class Coroutine {
public:
    inline static thread_local Coroutine* curr_co = nullptr;

public:
    Coroutine() = default;

    explicit Coroutine(std::function<void()> f) {
        ctx_ = boost::context::callcc([f = std::move(f), this](boost::context::continuation&& c) {
            suspend_ = [&c] {
                if (c) {
                    c = c.resume();
                } else {
                    throw std::runtime_error("");
                }
            };
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
        tx_ = ctx_.resume();
    }

    void suspend() {
        curr_co = nullptr;
        suspend_();
    }

    ~Coroutine() {
        curr_co = nullptr;
    }

private:
    boost::context::continuation ctx_;
    std::function<void()> suspend_;
};

void suspend() {
    if (Coroutine::curr_co == nullptr) {
        throw std::runtime_error("suspend with no coroutine");
    }

    Coroutine::curr_co->suspend();
}

template <typename T>
class Yield {
public:
    Yield() = default;
    Yield(std::unique_ptr<Coroutine> co) : co_(std::move(co)) {
    }

    template <typename U>
    void yield(U&& u) {
        value_ = std::make_unique<T>(std::move(u));
        suspend();
    }

    const T* GetValue() {
        return value_.release();
    }

    std::unique_ptr<Coroutine>& GetCoroutine() {
        return co_;
    }

    Yield& GetThis() {
        return *this;
    }

private:
    std::unique_ptr<T> value_{};
    std::unique_ptr<Coroutine> co_{};
};

template <typename T>
class Generator : public Yield<T> {
public:
    Generator(std::function<void(Yield<T>&)> f)
        : Yield<T>(std::make_unique<Coroutine>([f = std::move(f), this] {
              suspend();
              f(this->GetThis());
          })) {
    }

    const T* get() {
        try {
            this->GetCoroutine()->resume();
        } catch (...) {
            return nullptr;
        }

        return this->GetValue();
    }
};
c
