#pragma once

#include <memory>
#include <type_traits>

class Any {
    class InnerTypeBase {
    public:
        virtual ~InnerTypeBase() = default;
    };

    template <typename T>
    class InnerType : public InnerTypeBase {
    public:
        InnerType(const T& value) : value_(value) {
        }

        T& Get() {
            return value_;
        }

    private:
        T value_;
    };

public:
    Any() = default;

    template <class T>
    Any(const T& value) : data_(std::make_shared<InnerType<T>>(value)) {
    }

    template <class T>
    Any& operator=(const T& value) {
        data_.reset(new InnerType<T>(value));

        return *this;
    }

    Any(const Any& rhs) : data_(rhs.data_) {
    }
    Any& operator=(const Any& rhs) {
        auto copy(rhs);
        Swap(copy);

        return *this;
    }
    ~Any() = default;

    bool Empty() const {
        return (data_ == nullptr);
    }

    void Clear() {
        data_.reset();
    }
    void Swap(Any& rhs) {
        std::swap(data_, rhs.data_);
    }

    template <class T>
    const T& GetValue() const {
        if (std::is_convertible_v<T, InnerType<T>>) {
            return dynamic_cast<InnerType<T>&>(*data_).Get();
        }
        throw std::bad_cast{};
    }

private:
    std::shared_ptr<InnerTypeBase> data_{};
};
