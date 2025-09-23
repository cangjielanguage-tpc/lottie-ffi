/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LOTTIETURBO_CACHE_H
#define LOTTIETURBO_CACHE_H

#include <cstddef>
#include <list>
#include <mutex>

namespace LottieTurbo {
template<class Value>
class LottieCaches {
public:
    LottieCaches()
    {
    }
    ~LottieCaches()
    {
    }
    size_t Size() const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.size();
    }
    void Put(const std::string &key, Value &value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            it->second->second = value;
            list_.splice(list_.begin(), list_, it->second);
            return;
        }
        list_.emplace_front(key, value);
        map_[key] = list_.begin();
    }

    void Remove(std::string &key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            list_.erase(it->second);
            map_.erase(it);
        }
    }

    void RemoveAll()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        map_.clear();
        list_.clear();
    }
    
    Value Get(const std::string &key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it == map_.end()) {
            return Value(nullptr);
        }
        list_.splice(list_.begin(), list_, it->second);
        return it->second->second;
    }

    bool Contain(std::string &key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = map_.find(key);
        if (it != map_.end()) {
            return true;
        }
        return false;
    }

    std::pair<std::string, Value> GetTail()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (list_.empty()) {
            return std::pair<std::string, Value>("", Value());
        }
        return list_.back();
    }

    void PopTail()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (list_.empty()) {
            return ;
        }
        auto it = list_.back();
        map_.erase(it.first);
        list_.pop_back();
    }
private:
    LottieCaches(LottieCaches &) = delete;
    LottieCaches operator=(LottieCaches &) = delete;
    LottieCaches(LottieCaches &&) = default;
    LottieCaches& operator=(LottieCaches &&) = default;
    std::list<std::pair<std::string, Value>> list_;
    std::unordered_map<std::string, typename std::list<std::pair<std::string, Value>>::iterator> map_;
    mutable std::mutex mutex_;
};
}

#endif