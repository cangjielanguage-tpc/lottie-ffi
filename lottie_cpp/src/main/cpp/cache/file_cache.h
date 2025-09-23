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
#ifndef LOTTIETURBO_FILE_CACHE_H
#define LOTTIETURBO_FILE_CACHE_H

#include "cache/lottie_cache.h"

namespace LottieTurbo {
class FileCache {
public:
    enum class FileState {
        IDLE,
        READING,
        DELETING,
        WRITING
    };
    struct FileInfo {
        std::string fileName{""};
        size_t fileSize{0};
        std::atomic<FileState> state{FileState::IDLE};
        std::atomic<size_t> count{0};
        bool mark{false};
    };
    static FileCache *GetInstance(void);
    bool SetCacheSize(size_t size, size_t memorySize);
    bool Put(const std::string &fileName, std::shared_ptr<uint8_t> data, uint64_t size);
    bool RemoveFile(std::string &fileName);
    bool RemoveAllFile(void);
    std::pair<std::shared_ptr<uint8_t>, uint32_t> GetFileData(std::string &fileName);
    const std::string GetFileCachePath() const;
    const std::string GetTmpDirectoryPath() const;
    void InitFileCache();
    void DeleteTempDir(std::string tmpDir);
    void AddTempDir(std::string tmpDir);
    bool IsExistTempDir(std::string tmpDir);
    void ClearTmpFile();

private:
    ~FileCache();
    FileCache();
    bool WriteDataToDisk(std::shared_ptr<uint8_t> data, std::shared_ptr<FileInfo> fileInfo);
    // 这里可以进行扩展，用户自定义存储路径，当前仅使用默认路径
    const std::string defaultFileCachePath_{"/data/storage/el2/base/haps/entry/files/lottieCache/"};
    const std::string defaultTempPath_{"/data/storage/el2/base/haps/entry/files/lottietmp/"};
    const size_t defaultMaxSize_{20};
    const size_t defaultMaxMemory_{1024 * 1024 * 512};
    size_t maxSize_{defaultMaxSize_};
    size_t maxMemory_{defaultMaxMemory_};
    std::atomic<size_t> currentMemory_{0};
    LottieCaches<std::shared_ptr<FileInfo>> cache_;
    static FileCache *fileInstance_;
    bool isForbidden_{false};
    std::unordered_map<std::string, size_t> tmpDirRecods;
    bool isInitCache_{false};
};

} // namespace LottieTurbo

#endif