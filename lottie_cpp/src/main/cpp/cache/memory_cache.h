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

#ifndef LOTTIETURBO_MEMORY_CACHE_H
#define LOTTIETURBO_MEMORY_CACHE_H

#include "cache/file_cache.h"

namespace LottieTurbo {
class MemoryCache {
public:
    struct LottieDataInfo {
        std::string fileResourcePath{""};
        int64_t fileSize{0};
        std::shared_ptr<uint8_t> data{nullptr};
        bool mark{false};
    };
    static MemoryCache *GetInstance();
    bool SetMemoryCache(size_t number, uint64_t capacity);
    bool Put(std::string key, std::shared_ptr<LottieDataInfo> value);
    bool Remove(std::string key);
    bool RemoveAll(void);
    std::shared_ptr<LottieDataInfo> GetData(std::string fileName, bool isKey = false);
    bool ReadMemoryFromFileCache(std::shared_ptr<FileCache::FileInfo> info);

private:
    typedef LottieCaches<std::shared_ptr<LottieDataInfo>> CacheType;
    MemoryCache() = default;
    ~MemoryCache();
    static MemoryCache *memoryInstance_;
    const size_t defaultMaxNumber_{20};
    const int64_t defaultMaxCapacity_{128 * 1024 * 1024};
    size_t maxNumber_{defaultMaxNumber_};
    int64_t maxCapacity_{defaultMaxCapacity_};
    std::atomic<int64_t> currentCapacity_{0};
    CacheType cache_;
    bool isForbidden_{false};
};
} // namespace LottieC

#endif