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
#include "dataManager/data_parse.h"
#include "boundscheck/securec.h"
#include "dataManager//data_manager.h"
#include "cache/file_cache.h"
#include "cache/generator_key.h"
#include "cache/memory_cache.h"
#include "common/common.h"
#include "rlottie/src/lottie/zip/zip.h"
#include <filesystem>
#include <resourcemanager/ohresmgr.h>
#include <sys/stat.h>

namespace LottieTurbo {

JsonData::JsonData()
{
    std::filesystem::path tmpPath = FileCache::GetInstance()->GetTmpDirectoryPath().c_str();
    if (!std::filesystem::exists(tmpPath)) {
        std::filesystem::create_directories(tmpPath);
    }
}

bool JsonData::CheckIsZipHead(std::shared_ptr<uint8_t> data)
{
    const int head[] = {0x50, 0x4B, 0x03, 0x04};
    int size = sizeof(head) / sizeof(head[0]);
    for (int i = 0; i < size; i++) {
        if (data.get()[i] != head[i]) {
            return false;
        }
    }
    return true;
}

bool JsonData::GetMemoryData()
{
    // 如果内存缓存有，直接取内存数据
    auto memoryData = MemoryCache::GetInstance()->GetData(key_, true);
    if (memoryData != nullptr) {
        size_ = memoryData->fileSize;
        jsonData_ = memoryData->data;
        resourcePath_ = memoryData->fileResourcePath;
        return true;
    }
    return false;
}

bool JsonData::SaveDataToMemory()
{
    if (!useCache_) {
        return false;
    }
    // 缓存数据到内存
    auto dataInfo = std::make_shared<MemoryCache::LottieDataInfo>();
    dataInfo->fileSize = size_;
    dataInfo->data = jsonData_;
    dataInfo->mark = true;
    MemoryCache::GetInstance()->Put(key_, dataInfo);
    return true;
}

bool JsonData::ExtractJsonFromZip(const std::string& tmpPath, uint8_t*& data, uint64_t& bytes)
{
    ::zip_t *zip = ::zip_stream_open((char *)jsonData_.get(), size_, 0, 'r');
    int total = ::zip_entries_total(zip);
    bool flag = false;

    for (int i = 0; i < total; i++) {
        if (::zip_entry_openbyindex(zip, i) < 0) {
            continue;
        }
        std::string name = ::zip_entry_name(zip);
        int ret = name.find("MACOS");
        if (ret >= 0) {
            continue;
        }
        ret = name.find(".json");
        if (ret < 0) {
            continue;
        }
        size_t size = ::zip_entry_size(zip);
        data = new uint8_t[sizeof(uint8_t) * size];
        if (data != nullptr) {
            bytes = ::zip_entry_read(zip, (void **)&data, &size);
            int index = name.find_last_of("/");
            resourcePath_ = tmpPath;
            int i;
            for (i = 0; i < index; i++) {
                resourcePath_ += name[i];
            }
            if (i >= index) {
                resourcePath_ += "/";
            }
        }
        ::zip_entry_close(zip);
        flag = true;
        break;
    }
    ::zip_close(zip);
    return flag;
}

std::pair<std::shared_ptr<uint8_t>, uint64_t> JsonData::GetData()
{
    const int minSize = 4;
    uint8_t *data = nullptr;
    uint64_t bytes = 0;
    if (size_ < minSize) {
        return std::pair<std::shared_ptr<uint8_t>, uint64_t>(jsonData_, size_);
    }
    if (CheckIsZipHead(jsonData_) && resourcePath_ == "") {
        std::string tmp = FileCache::GetInstance()->GetTmpDirectoryPath() + key_ + "/";
        if (!FileCache::GetInstance()->IsExistTempDir(tmp)) {
            ::zip_stream_extract((char *)jsonData_.get(), size_, tmp.c_str(), nullptr, nullptr);
        }
        FileCache::GetInstance()->AddTempDir(tmp);
        filePath_ = tmp;
        if (ExtractJsonFromZip(tmp, data, bytes)) {
            std::shared_ptr<uint8_t> ptr(data, [](uint8_t *data) { delete[] data; });
            return std::pair<std::shared_ptr<uint8_t>, uint64_t>(std::move(ptr), bytes);
        }
    }
    return std::pair<std::shared_ptr<uint8_t>, uint64_t>(jsonData_, size_);
}

std::string JsonData::GetResourcePath()
{
    return resourcePath_;
}

std::string JsonData::GetFilePath()
{
    return filePath_;
}

void JsonData::SetUseCache(bool useCache)
{
    useCache_ = useCache;
}

bool CommonParse::Resolve()
{
    if (type_ != ResourcesType::COMMON) {
        return false;
    }
    if (useCache_ && GetMemoryData()) {
        return true;
    }
    if (LoadJsonFromCommon()) {
        SaveDataToMemory();
        return true;
    }
    return false;
}

CommonParse::CommonParse(std::string path)
{
    path_ = path;
    type_ = ResourcesType::COMMON;
    key_ = GeneratorKey(path_);
}

CommonParse::~CommonParse()
{
}

bool CommonParse::LoadJsonFromCommon()
{
    int ret = path_.find("/data/storage");
    if (ret == 0) {
        return LoadJsonFromSandbox();
    }
    return LoadJsonFromNetwork();
}

bool CommonParse::LoadJsonFromSandbox()
{
    struct stat fileStat;
    int ret = stat(path_.c_str(), &fileStat);
    if (ret == -1) {
        return false;
    }

    FILE *fd = fopen(path_.c_str(), "r");
    if (fd == nullptr) {
        return false;
    }

    uint8_t *buffer = new uint8_t[(fileStat.st_size + 1) * sizeof(uint8_t)];
    uint64_t length = fread(buffer, sizeof(uint8_t), fileStat.st_size, fd);
    if (fclose(fd) != 0) {
        // 由于文件数据已经读取完成，即使关闭失败也不影响后续逻辑，所以记录错误并继续执行是合理的
        DRAWING_LOGE("CommonParse::LoadJsonFromSandbox: File close error for %{public}s", path_.c_str());
    }
    fd = nullptr;
    if (length == fileStat.st_size) {
        buffer[length] = 0;
    } else {
        // 处理部分读取或失败
        DRAWING_LOGE("CommonParse::LoadJsonFromSandbox: Read file went wrong!");
        delete [] buffer;
        return false;
    }
    std::shared_ptr<uint8_t> ptr(buffer, [](uint8_t *data) {
        delete[] data;
        data = nullptr;
    });
    jsonData_ = std::move(ptr);
    size_ = length;
    return true;
}

bool CommonParse::LoadJsonFromNetwork()
{
    // 如果本地有文件緩存，直接从缓存中读取
    if (useCache_) {
        // 如果本地有文件緩存，直接从缓存中读取
        auto fileData = FileCache::GetInstance()->GetFileData(path_);
        if (fileData.second > 0) {
            size_ = fileData.second;
            jsonData_ = std::move(fileData.first);
            return true;
        }
    }

    FileDownload fileDownload;
    std::shared_ptr<uint8_t> jsonData{nullptr};
    size_t size;
    auto exec = [&jsonData, &size](Rcp_Response *data) {
        if (!data || !data->body.buffer || data->body.length == 0) {
            return;
        }
        uint8_t *buffer = new uint8_t[data->body.length];
        if (!buffer) {
            return;
        }
        errno_t result = memcpy_s(buffer, data->body.length, data->body.buffer, data->body.length);
        std::shared_ptr<uint8_t> ptr(buffer, [](uint8_t *buf) {
            delete[] buf;
            buf = nullptr;
        });
        if (result != 0) {
            DRAWING_LOGE("CommonParse::LoadJsonFromNetwork: memcpy_s failed (error %d)", result);
            return;
        }
        jsonData = std::move(ptr);
        size = data->body.length;
    };
    fileDownload.Start(path_, exec);
    fileDownload.Sync();
    if (jsonData != nullptr && size > 0) {
        size_ = size;
        jsonData_ = std::move(jsonData);
        FileCache::GetInstance()->Put(path_, jsonData_, size_);
        return true;
    }
    return false;
}

bool ResourceParse::Resolve()
{
    if (type_ != ResourcesType::RESOURCE) {
        return false;
    }
    if (GetMemoryData()) {
        return true;
    }
    if (LoadJsonFromResource()) {
        SaveDataToMemory();
        return true;
    }
    return false;
}

ResourceParse::ResourceParse(Resource resource, std::shared_ptr<NativeResourceManager> resourceManager)
{
    resource_ = resource;
    type_ = ResourcesType::RESOURCE;
    resourceManager_ = resourceManager;
    std::string keyStr = resource_.moduleName;
    keyStr += resource_.bundleName;
    keyStr += std::to_string(resource_.number);
    keyStr += std::to_string(resource_.id);
    keyStr += resource_.params;
    key_ = GeneratorKey(keyStr);
}

ResourceParse::~ResourceParse()
{
}

bool ResourceParse::LoadJsonFromResource()
{
    ResourceManager_ErrorCode errCode;
    uint8_t *buffer = nullptr;
    uint64_t length = 0;
    if (resourceManager_ == nullptr) {
        return false;
    }
    
    if (resource_.id == -1) {
        errCode = LoadJsonFromOtherModule(&buffer, length);
    }

    if (errCode != ResourceManager_ErrorCode::SUCCESS) {
        errCode = LoadJsonFromRawfile(&buffer, length);
    }
    std::shared_ptr<uint8_t> ptr(buffer, [](uint8_t *data) {
        free(data);
        data = nullptr;
    });

    if (errCode == ResourceManager_ErrorCode::SUCCESS && length > 0) {
        size_ = length;
        jsonData_ = std::move(ptr);
        return true;
    }
    return false;
}

ResourceManager_ErrorCode ResourceParse::LoadJsonFromOtherModule(uint8_t **buffer, uint64_t &length)
{
    const char *params = resource_.params.c_str();
    const size_t maxFileNameLength = 512;
    char fileName[maxFileNameLength] = {0};
    size_t fileLength = -1;
    size_t index = 0;
    // [modeule].xxx.json, 目前目录结构要求的格式
    for (int i = resource_.params.size() - 1; i >= 0; i--) {
        if (params[i] == ']') {
            index = i + 1 + 1;
            break;
        }
    }
    for (int i = index; i < resource_.params.size(); i++) {
        fileName[++fileLength] = params[i];
    }
    fileName[++fileLength] = 0;
    resource_.params = fileName;
    return LoadJsonFromRawfile(buffer, length);
}

ResourceManager_ErrorCode ResourceParse::LoadJsonFromRawfile(uint8_t **buffer, uint64_t &length)
{
    RawFile64 *rFile = OH_ResourceManager_OpenRawFile64(resourceManager_.get(), resource_.params.c_str());
    length = 0;
    if (rFile != nullptr) {
        uint64_t len = OH_ResourceManager_GetRawFileRemainingLength64(rFile);
        if (*buffer == nullptr) {
            *buffer = (uint8_t *)malloc((len + 1) * sizeof(uint8_t));
        }
        length = OH_ResourceManager_ReadRawFile64(rFile, *buffer, len);
        (*buffer)[length] = 0;
        OH_ResourceManager_CloseRawFile64(rFile);
        rFile = nullptr;
        if (length > 0) {
            return ResourceManager_ErrorCode::SUCCESS;
        }
    }
    return ResourceManager_ErrorCode::ERROR_CODE_RES_ID_NOT_FOUND;
}

FileDownload::FileDownload()
{
    uint32_t errCode = 0;
    rcpSession_ = HMS_Rcp_CreateSession(NULL, &errCode);
}

FileDownload::~FileDownload()
{
    Sync();
    if (rcpSession_ != nullptr) {
        HMS_Rcp_CloseSession(&rcpSession_);
        rcpSession_ = nullptr;
    }
}

bool FileDownload::Start(std::string url, std::function<void(Rcp_Response *)> func)
{
    CallbackData *callbackData = new CallbackData;
    callbackData->request = HMS_Rcp_CreateRequest(url.c_str());
    callbackData->request->method = RCP_METHOD_GET;
    callbackData->func = func;
    callbackData->promise = new std::promise<std::string>();
    queque_.push(callbackData);
    // 这里不能使用局部变量，该函数为非阻塞的，考虑将地址给到callbackdata里面，后续等操作完后回收内存
    Rcp_ResponseCallbackObject *cobject = new Rcp_ResponseCallbackObject;
    cobject->callback = ResponseCallback;
    cobject->usrCtx = callbackData;
    callbackData->callbackObject = cobject;
    HMS_Rcp_Fetch(rcpSession_, callbackData->request, cobject);
    return true;
}

void FileDownload::ResponseCallback(void *usrCtx, Rcp_Response *response, uint32_t errCode)
{
    CallbackData *callbackData = reinterpret_cast<CallbackData *>(usrCtx);
    if (callbackData == nullptr) {
        return;
    }
    if (response != nullptr) {
        callbackData->func(response);
        response->destroyResponse(response);
    }
    callbackData->promise->set_value("ok");
}

bool FileDownload::Sync()
{
    while (!queque_.empty()) {
        auto top = queque_.front();
        queque_.pop();
        top->promise->get_future().get();
        HMS_Rcp_DestroyRequest(top->request);
        delete top->promise;
        top->promise = nullptr;

        delete top->callbackObject;
        top->callbackObject = nullptr;

        delete top;
        top = nullptr;
    }
    return true;
}
}