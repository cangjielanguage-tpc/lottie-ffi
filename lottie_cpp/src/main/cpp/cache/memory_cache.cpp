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

#include "cache/memory_cache.h"
#include "cache/generator_key.h"
#include "common/common.h"
#include <filesystem>

namespace LottieTurbo {
MemoryCache *MemoryCache::memoryInstance_ = nullptr;

MemoryCache *MemoryCache::GetInstance()
{
    if (memoryInstance_ == nullptr) {
        memoryInstance_ = new MemoryCache();
    }
    return memoryInstance_;
}

MemoryCache::~MemoryCache()
{
    if (memoryInstance_ != nullptr) {
        delete memoryInstance_;
        memoryInstance_ = nullptr;
    }
}

bool MemoryCache::SetMemoryCache(size_t number, uint64_t capacity)
{
    maxNumber_ = number;
    maxCapacity_ = capacity;
    if (number == 0 || capacity == 0) {
        isForbidden_ = true;
    } else {
        isForbidden_ = false;
    }
    // 改变了cache大小，需要刷新下cache，推个空数据触发刷新lru
    Put("", nullptr);
    return true;
}

bool MemoryCache::Put(std::string fileName, std::shared_ptr<LottieDataInfo> info)
{
    if (isForbidden_) {
        return false;
    }
    std::string key = "";
    if (info != nullptr && info->mark) {
        key = fileName;
    } else {
        key = GeneratorKey(fileName);
    }

    if (cache_.Contain(key)) {
        return true;
    }

    while (currentCapacity_ >= maxCapacity_ || cache_.Size() >= maxNumber_) {
        auto value = cache_.GetTail();
        if (value.second != nullptr) {
            currentCapacity_ -= value.second->fileSize;
        }
        if (value.first != "") {
            cache_.Remove(value.first);
        } else {
            break;
        }
    }

    if (fileName == "") {
        return false;
    }

    cache_.Put(key, info);
    currentCapacity_ += info->fileSize;
    return true;
}

bool MemoryCache::Remove(std::string fileName)
{
    if (isForbidden_) {
        return false;
    }
    std::string key = GeneratorKey(fileName);
    cache_.Remove(key);
    return true;
}

bool MemoryCache::RemoveAll(void)
{
    if (isForbidden_) {
        return false;
    }
    cache_.RemoveAll();
    return true;
}

std::shared_ptr<MemoryCache::LottieDataInfo> MemoryCache::GetData(std::string fileName, bool isKey)
{
    if (isForbidden_) {
        return nullptr;
    }
    std::string key = isKey ? fileName : GeneratorKey(fileName);
    std::shared_ptr<LottieDataInfo> info = cache_.Get(key);
    return info;
}

bool MemoryCache::ReadMemoryFromFileCache(std::shared_ptr<FileCache::FileInfo> info)
{
    std::string fileCachePath = FileCache::GetInstance()->GetFileCachePath();
    std::string key = "";
    if (info->mark) {
        key = info->fileName;
    } else {
        key = GeneratorKey(info->fileName);
    }
    FILE *f = fopen((fileCachePath + key).c_str(), "rb");
    if (f == nullptr) {
        return false;
    }
    uint8_t *buffer = new uint8_t[sizeof(uint8_t) * info->fileSize];
    size_t elementsRead = fread(buffer, sizeof(uint8_t), info->fileSize, f);
    if (elementsRead != info->fileSize) {
        DRAWING_LOGE("ReadMemoryFromFileCache: Failed to read file");
        delete[] buffer;

        // 即使读取失败也要尝试关闭文件
        if (fclose(f) != 0) {
            DRAWING_LOGE("ReadMemoryFromFileCache: Failed to close file: %{public}s after read error",
                         (fileCachePath + key).c_str());
        }
        return false;
    }

    // 处理fclose返回值
    if (fclose(f) != 0) {
        DRAWING_LOGE("ReadMemoryFromFileCache: Failed to close file: %{public}s", (fileCachePath + key).c_str());
        delete[] buffer;
        return false;
    }
    f = nullptr;
    std::shared_ptr<uint8_t> data(buffer, [](uint8_t *data) { delete[] data; });
    auto ptr = std::make_shared<MemoryCache::LottieDataInfo>();
    ptr->fileSize = info->fileSize;
    ptr->data = std::move(data);
    ptr->mark = info->mark;
    return MemoryCache::GetInstance()->Put(info->fileName, std::move(ptr));
}
}