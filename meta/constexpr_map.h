#pragma once

#include <stdexcept>
#include <array>
#include <utility>

template <class K, class V, int MaxSize = 8>
class ConstexprMap {
public:
    constexpr ConstexprMap() = default;

    constexpr V& operator[](const K& key) {
        bool has = Find(key);

        if (!has && size_ == MaxSize) {
            throw std::out_of_range("map is full");
        }

        if (has) {
            for (size_t i = 0; i < size_; ++i) {
                if (keys_values_[i].first == key) {
                    return keys_values_[i].second;
                }
            }
        }
        keys_values_[size_].first = key;
        return keys_values_[size_++].second;
    }

    constexpr const V& operator[](const K& key) const {
        for (size_t i = 0; i < size_; ++i) {
            if (keys_values_[i].first == key) {
                return keys_values_[i].second;
            }
        }

        throw std::out_of_range("no such element");
    }

    constexpr bool Erase(const K& key) {
        if (!Find(key)){
            return false;
        }

        size_t idx = 0;
        for (;; ++idx) {
            if (keys_values_[idx].first == key) {
                break;
            }
        }

        for (size_t i = idx; i + 1 < size_; ++i) {
            keys_values_[i] = keys_values_[i + 1];
        }
        size_ -= 1;

        return true;
    }

    constexpr bool Find(const K& key) const {
        for (size_t i = 0; i < size_; ++i) {
            if (keys_values_[i].first == key) {
                return true;
            }
        }

        return false;
    }

    constexpr size_t Size() const {
        return size_;
    }

    constexpr std::pair<K, V>& GetByIndex(size_t pos) {
        return keys_values_[pos];
    }

    constexpr const std::pair<K, V>& GetByIndex(size_t pos) const {
        return keys_values_[pos];
    }

private:
    std::array<std::pair<K, V>, MaxSize> keys_values_;
    size_t size_{};
};
