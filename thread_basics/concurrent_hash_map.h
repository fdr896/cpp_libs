#pragma once

#include <vector>
#include <list>
#include <memory>
#include <mutex>
#include <functional>
#include <atomic>
#include <stdexcept>
#include <iostream>

template <class K, class V, class Hash = std::hash<K>>
class ConcurrentHashMap {
public:
    inline static constexpr int kLoadFactor = 2;
    inline static constexpr int kDefaultConcurrencyLevel = 32;
    inline static constexpr int kUndefinedSize = 64;

    ConcurrentHashMap(const Hash& hasher = Hash()) : ConcurrentHashMap(kUndefinedSize, hasher) {
    }

    explicit ConcurrentHashMap(int expected_size, const Hash& hasher = Hash())
        : ConcurrentHashMap(expected_size, kDefaultConcurrencyLevel, hasher) {
    }

    ConcurrentHashMap(int expected_size, int expected_threads_count, const Hash& hasher = Hash())
        : hasher_(hasher) {
        size_.store(0u);
        expected_threads_count = std::min(4, expected_threads_count);

        concure_level_ = expected_threads_count * 4;

        if (expected_size <= concure_level_ * kLoadFactor) {
            cap_ = concure_level_ * kLoadFactor;
        } else {
            cap_ = (expected_size / concure_level_ + 1) * concure_level_ * kLoadFactor;
        }

        data_.resize(cap_);
    }

    bool Insert(const K& key, const V& value) {
        Rehash();

        auto hash = hasher_(key);
        std::lock_guard lock(mutexes_[hash % concure_level_]);
        hash %= cap_;

        if (auto it = std::find_if(data_[hash].begin(), data_[hash].end(),
                                   [key](const auto& p) { return p.first == key; });
            it != data_[hash].end()) {
            return false;
        }

        size_.fetch_add(1);
        data_[hash].emplace_back(key, value);

        return true;
    }

    bool Erase(const K& key) {
        auto hash = hasher_(key);
        std::lock_guard lock(mutexes_[hash % concure_level_]);
        hash %= cap_;

        auto it = std::find_if(data_[hash].begin(), data_[hash].end(),
                               [key](const auto& p) { return p.first == key; });

        if (it == data_[hash].end()) {
            return false;
        }

        size_.fetch_sub(1);
        data_[hash].erase(it);
        return true;
    }

    void Clear() {
        for (int i = 0; i < concure_level_; ++i) {
            mutexes_[i].lock();
        }

        size_.store(0);
        cap_ = concure_level_ * kLoadFactor * kLoadFactor;
        data_.clear();
        data_.resize(cap_);

        for (int i = concure_level_ - 1; i >= 0; --i) {
            mutexes_[i].unlock();
        }
    }

    std::pair<bool, V> Find(const K& key) const {
        auto hash = hasher_(key);
        std::lock_guard lock(mutexes_[hash % concure_level_]);
        hash %= cap_;

        auto it = std::find_if(data_[hash].begin(), data_[hash].end(),
                               [key](const auto& p) { return p.first == key; });

        if (it == data_[hash].end()) {
            return {false, V()};
        }
        return {true, it->second};
    }

    const V At(const K& key) const {
        auto [has, value] = Find(key);

        if (!has) {
            throw std::out_of_range{"no such key"};
        }
        return value;
    }

    size_t Size() const {
        return size_.load();
    }

private:
    void Rehash() {
        if (size_.load() * 4 > cap_) {
            for (int i = 0; i < concure_level_; ++i) {
                mutexes_[i].lock();
            }

            size_t new_cap = cap_ * kLoadFactor;
            std::vector<std::list<std::pair<K, V>>> new_data(new_cap);
            for (size_t i = 0; i < cap_; ++i) {
                for (const auto& [key, value] : data_[i]) {
                    auto hash = hasher_(key) % new_cap;

                    new_data[hash].emplace_back(key, value);
                }
            }

            cap_ = new_cap;
            data_.swap(new_data);
            for (int i = concure_level_ - 1; i >= 0; --i) {
                mutexes_[i].unlock();
            }
        }
    }

    Hash hasher_;
    std::atomic<size_t> cap_{};
    int concure_level_{};
    std::atomic<size_t> size_{};
    std::vector<std::list<std::pair<K, V>>> data_;
    mutable std::mutex mutexes_[kDefaultConcurrencyLevel];
};
