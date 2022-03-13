#pragma once

#include <string>
#include <list>
#include <unordered_map>

class LruCache {
public:
    LruCache(size_t max_size);

    void Set(const std::string& key, const std::string& value);

    bool Get(const std::string& key, std::string* value);

private:
    size_t cache_;
    std::list<std::pair<std::string, std::string>> values_;
    std::unordered_map<std::string, std::list<std::pair<std::string, std::string>>::iterator>
        links_;
};

LruCache::LruCache(size_t max_size) : cache_(max_size) {
}

void LruCache::Set(const std::string& key, const std::string& value) {
    auto it = links_.find(key);
    if (it != links_.end()) {
        values_.erase(it->second);
        links_.erase(it);
    }
    values_.push_front({key, value});
    links_[key] = values_.begin();

    if (values_.size() > cache_) {
        auto delete_key = values_.back().first;
        values_.pop_back();
        links_.erase(delete_key);
    }
}

bool LruCache::Get(const std::string& key, std::string* value) {
    auto it = links_.find(key);
    if (it == links_.end()) {
        return false;
    }

    auto it_list = it->second;
    std::string val = it_list->second;
    *value = val;
    values_.erase(it_list);
    values_.push_front({key, val});
    links_.erase(val);
    links_[key] = values_.begin();

    return true;
}

