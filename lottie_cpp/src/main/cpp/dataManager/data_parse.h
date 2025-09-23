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

#ifndef LOTTIETURBO_DATA_PARSE_H
#define LOTTIETURBO_DATA_PARSE_H

#include "zip/zip.h"
#include <RemoteCommunicationKit/rcp.h>
#include <future>
#include <queue>
#include <rawfile/raw_file_manager.h>
#include <resourcemanager/resmgr_common.h>

namespace LottieTurbo {

enum class ResourcesType {
    COMMON,
    RESOURCE,
    NONE
};

struct Resource {
    std::string bundleName{""};
    std::string moduleName{""};
    int id{-2};
    int number{0};
    std::string params{""};
};

class JsonData {
public:
    virtual bool Resolve() = 0;
    virtual ~JsonData(){};
    std::pair<std::shared_ptr<uint8_t>, uint64_t> GetData();
    std::string GetResourcePath();
    std::string GetFilePath();
    void SetUseCache(bool useCache);

protected:
    JsonData();
    bool GetMemoryData();
    bool SaveDataToMemory();
    // 检查数据是否为ZIP文件头
    bool CheckIsZipHead(std::shared_ptr<uint8_t> data);
    // 从ZIP文件中提取JSON数据
    bool ExtractJsonFromZip(const std::string& tmpPath, uint8_t*& data, uint64_t& bytes);
    std::shared_ptr<uint8_t> jsonData_{nullptr};
    uint64_t size_{0};
    std::string resourcePath_{""};
    std::string key_{""};
    ResourcesType type_{ResourcesType::NONE};
    // 文件解压路径不等于资源路径，这里要区分开
    std::string filePath_{""};
    bool useCache_{true};
};

class CommonParse : public JsonData {
public:
    bool Resolve() override;
    CommonParse(std::string path);
    ~CommonParse();

private:
    bool LoadJsonFromCommon();
    bool LoadJsonFromSandbox();
    bool LoadJsonFromNetwork();

    std::string path_;
};

class ResourceParse : public JsonData {
public:
    bool Resolve() override;
    ResourceParse(Resource resource, std::shared_ptr<NativeResourceManager> resourceManager);
    ~ResourceParse();

private:
    bool LoadJsonFromResource();
    ResourceManager_ErrorCode LoadJsonFromOtherModule(uint8_t **buffer, uint64_t &length);
    ResourceManager_ErrorCode LoadJsonFromRawfile(uint8_t **buffer, uint64_t &length);
    Resource resource_;
    std::shared_ptr<NativeResourceManager> resourceManager_{nullptr};
};

class FileDownload {
public:
    FileDownload();
    ~FileDownload();
    bool Start(std::string url, std::function<void(Rcp_Response *)> func);
    bool Sync();

private:
    struct CallbackData {
        Rcp_Request *request{nullptr};
        std::promise<std::string> *promise{nullptr};
        std::function<void(Rcp_Response *)> func;
        Rcp_ResponseCallbackObject *callbackObject{nullptr};
    };
    static void ResponseCallback(void *usrCtx, Rcp_Response *response, uint32_t errCode);
    std::queue<CallbackData *> queque_;
    Rcp_Session *rcpSession_{nullptr};
};
} // namespace LottieTurbo

#endif