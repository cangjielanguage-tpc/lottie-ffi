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
#include "cache/file_cache.h"
#include "cache//generator_key.h"
#include "cache/memory_cache.h"
#include "common/common.h"
#include "task_pool/task_pool.h"
#include <cstdio>
#include <dirent.h>
#include <filesystem>
#include <queue>
#include <sys/stat.h>

namespace LottieTurbo {
FileCache *FileCache::fileInstance_ = nullptr;

FileCache *FileCache::GetInstance(void)
{
    if (fileInstance_ == nullptr) {
        fileInstance_ = new FileCache();
    }
    return fileInstance_;
}

FileCache::FileCache()
{
    std::filesystem::path fileCachePath = defaultFileCachePath_.c_str();
    if (!std::filesystem::exists(fileCachePath)) {
        std::filesystem::create_directories(fileCachePath);
    }
}

FileCache::~FileCache()
{
    if (fileInstance_ != nullptr) {
        delete fileInstance_;
        fileInstance_ = nullptr;
    }
}

bool FileCache::SetCacheSize(size_t size, size_t memorySize)
{
    maxSize_ = size;
    maxMemory_ = memorySize;
    if (size == 0 || memorySize == 0) {
        isForbidden_ = true;
    } else {
        isForbidden_ = false;
    }
    // 改变了cache大小，需要刷新下cache，推个空数据触发刷新lru
    Put("", nullptr, 0);
    return true;
}

bool FileCache::Put(const std::string &fileName, std::shared_ptr<uint8_t> data, uint64_t size)
{
    if (isForbidden_) {
        return false;
    }

    std::string key = GeneratorKey(fileName);
    if (cache_.Contain(key)) {
        return true;
    }

    while (currentMemory_ >= maxMemory_ || cache_.Size() >= maxSize_) {
        std::shared_ptr<FileInfo> value = cache_.GetTail().second;
        if (value == nullptr) {
            break;
        }
        if (value->state == FileState::IDLE && value->fileName != "") {
            RemoveFile(value->fileName);
        }
    }
    if (fileName == "") {
        return false;
    }
    std::shared_ptr<FileInfo> fileInfo = std::make_shared<FileInfo>();
    fileInfo->fileName = fileName;
    fileInfo->fileSize = size;
    fileInfo->state = FileState::WRITING;
    cache_.Put(key, fileInfo);
    currentMemory_ += size;
    // 这里可以开线程，能优化掉写文件的时间
    WriteDataToDisk(data, fileInfo);
    return true;
}

static void TaskWriteCallback(std::string path, std::shared_ptr<uint8_t> data,
                              std::shared_ptr<FileCache::FileInfo> fileInfo)
{
    FILE *fp = fopen(path.c_str(), "wb");
    if (fp == nullptr) {
        DRAWING_LOGE("TaskWriteCallback: open file failed");
        return;
    }
    uint64_t length = fwrite(data.get(), sizeof(char), fileInfo->fileSize, fp);

    if (fclose(fp) != 0) {
        DRAWING_LOGE("TaskWriteCallback: Failed to close file: %{public}s", path.c_str());
        // 虽然文件关闭失败，但数据已读取，可以继续处理
    }
    fp = nullptr;
    if (length != fileInfo->fileSize) {
        DRAWING_LOGE("TaskWriteCallback: write file failed");
        return;
    }
    fileInfo->state = FileCache::FileState::IDLE;
}

bool FileCache::WriteDataToDisk(std::shared_ptr<uint8_t> data, std::shared_ptr<FileInfo> fileInfo)
{
    std::string key = GeneratorKey(fileInfo->fileName);
    std::string path = defaultFileCachePath_ + key;
    TaskPool::GetInstance().PushTask(ffrt_queue_priority_low, TaskWriteCallback, path, data, fileInfo);
    return true;
}

std::pair<std::shared_ptr<uint8_t>, uint32_t> FileCache::GetFileData(std::string &fileName)
{
    if (isForbidden_) {
        return std::make_pair(nullptr, 0);
    }

    std::string key = GeneratorKey(fileName);
    if (!cache_.Contain(key)) {
        return std::make_pair(nullptr, 0);
    }

    std::string path = defaultFileCachePath_ + key;
    std::shared_ptr<FileInfo> info = cache_.Get(key);
    if (info->state == FileState::WRITING || info->state == FileState::DELETING) {
        return std::make_pair(nullptr, 0);
    }
    info->state = FileState::READING;
    info->count++;
    uint8_t *buffer = new uint8_t[info->fileSize];
    std::shared_ptr<uint8_t> ptr(buffer, [](uint8_t *buffer) {
        delete[] buffer;
        buffer = nullptr;
    });

    FILE *fp = fopen(path.c_str(), "rb");
    if (fp == nullptr) {
        return std::make_pair(nullptr, 0);
    }
    uint64_t length = fread(buffer, sizeof(uint8_t), info->fileSize, fp);
    if (fclose(fp) != 0) {
        DRAWING_LOGE("FileCache::GetFileData: Failed to close file: %s", path.c_str());
        // 虽然文件关闭失败，但数据已读取，可以继续处理
    }
    fp = nullptr;
    if (length != info->fileSize) {
         // 处理部分读取或失败
        DRAWING_LOGE("FileCache::GetFileData: Read file went wrong!");
        return std::make_pair(nullptr, 0);
    }
    info->count--;
    if (info->count <= 0) {
        info->state = FileState::IDLE;
    }
    // 这里防止下载失败，或者写失败导致存了大小为0kb的文件，清除这种类型文件
    if (info->fileSize == 0) {
        RemoveFile(fileName);
        return std::make_pair(nullptr, 0);
    }

    auto memoryPtr = std::make_shared<MemoryCache::LottieDataInfo>();
    memoryPtr->fileSize = info->fileSize;
    memoryPtr->data = ptr;
    memoryPtr->mark = info->mark;
    MemoryCache::GetInstance()->Put(info->fileName, std::move(memoryPtr));
    return std::make_pair(ptr, info->fileSize);
}

bool FileCache::RemoveFile(std::string &fileName)
{
    if (isForbidden_) {
        return false;
    }

    std::string key = GeneratorKey(fileName);
    if (!cache_.Contain(key)) {
        // 这里如果非空，说明这个文件是之前内存缓存的文件,因为初始化读取目录的文件是没有原本的fileName的,只能读取key
        if (!cache_.Contain(fileName)) {
            return true;
        }
        key = fileName;
    }
    std::shared_ptr<FileInfo> info = cache_.Get(key);
    if (info->state == FileState::WRITING || info->state == FileState::READING) {
        return false;
    }
    if (info->state == FileState::DELETING) {
        return true;
    }

    std::string path = defaultFileCachePath_ + key;
    info->state = FileState::DELETING;
    std::filesystem::remove(path);
    currentMemory_ -= info->fileSize;
    cache_.Remove(key);
    return true;
}

bool FileCache::RemoveAllFile(void)
{
    if (isForbidden_) {
        return false;
    }
    int count = 0;
    int max = cache_.Size();
    std::shared_ptr<FileInfo> info = nullptr;
    while ((info = cache_.GetTail().second) != nullptr && count < max) {
        RemoveFile(info->fileName);
        count++;
    }
    return true;
}

void FileCache::InitFileCache()
{
    if (isInitCache_) {
        return;
    }
    DIR *dir = nullptr;
    struct dirent *entry;
    struct stat statbuf;
    dir = opendir(defaultFileCachePath_.c_str());
    if (dir == nullptr) {
        return;
    }
    while ((entry = readdir(dir)) != nullptr) {
        if (stat((defaultFileCachePath_ + entry->d_name).c_str(), &statbuf) == -1) {
            continue;
        }
        if (S_ISDIR(statbuf.st_mode)) {
            continue;
        }
        std::string key = entry->d_name;
        std::shared_ptr<FileInfo> info = std::make_shared<FileInfo>();
        info->fileName = entry->d_name;
        info->state = FileState::IDLE;
        info->fileSize = statbuf.st_size;
        info->mark = true;
        cache_.Put(key, info);
        currentMemory_ += info->fileSize;
        MemoryCache::GetInstance()->ReadMemoryFromFileCache(info);
    }
    isInitCache_ = true;
}
const std::string FileCache::GetFileCachePath() const
{
    return defaultFileCachePath_;
}
const std::string FileCache::GetTmpDirectoryPath() const
{
    return defaultTempPath_;
}
void FileCache::DeleteTempDir(std::string tmpDir)
{
    auto it = tmpDirRecods.find(tmpDir);
    if (it != tmpDirRecods.end()) {
        if (it->second > 1) {
            it->second--;
        } else {
            tmpDirRecods.erase(tmpDir);
            std::filesystem::remove_all(tmpDir);
        }
    }
}
void FileCache::AddTempDir(std::string tmpDir)
{
    auto it = tmpDirRecods.find(tmpDir);
    if (it == tmpDirRecods.end()) {
        tmpDirRecods.insert({tmpDir, 1});
    } else {
        it->second++;
    }
}

bool FileCache::IsExistTempDir(std::string tmpDir)
{
    auto it = tmpDirRecods.find(tmpDir);
    if (it == tmpDirRecods.end()) {
        return false;
    }
    return true;
}

void FileCache::ClearTmpFile()
{
    // 这里提供清除临时文件的接口，避免异常退出时，没有及时清理文件，导致越积越多
    // 如果在动画正在使用资源文件时调用，会被系统函数abort，确保没有动画渲染时调用
    if (std::filesystem::exists(FileCache::GetInstance()->GetTmpDirectoryPath())) {
        std::filesystem::remove_all(FileCache::GetInstance()->GetTmpDirectoryPath());
    }
}

}