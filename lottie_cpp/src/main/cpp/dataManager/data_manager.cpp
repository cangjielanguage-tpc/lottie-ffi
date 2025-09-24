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

#include "dataManager/data_manager.h"
#include "cangjie/animator.h"
#include "cache/file_cache.h"
#include "cache/generator_key.h"
#include "common/common.h"
#include "common/number_constant.h"
#include "lottiemodel.h"
#include <cstdint>

namespace LottieTurbo {

DataManager::~DataManager()
{
    player.reset();
}

void DataManager::InitRect(int32_t width, int32_t height)
{
    width_ = width;
    height_ = height;
}

void DataManager::UpdataImageAssets(std::shared_ptr<rlottie::internal::model::Composition> &composition)
{
    if (composition == nullptr) {
        DRAWING_LOGE("DataManager::UpdataImageAssets: composition or imgAssets_ is nullptr!");
        return;
    }
    if (imgAssets_ != nullptr) {
        const int dataLen = 5;
        const int httpLen = 4;
        for (int i = 0; i < imgAssets_->size(); ++i) {
            auto iter = composition->mAssets.find(imgAssets_->at(i).refId);
            if (iter == composition->mAssets.end()) {
                continue;
            }
            auto &fileName = imgAssets_->at(i).fileName;
            auto &assert = iter->second;
            if (fileName.compare(0, dataLen, "data:") == 0) {
                assert->loadImageData(rlottie::internal::model::convertFromBase64(fileName));
                assert->mUrlName.clear();
            } else if (fileName.compare(0, httpLen, "http") == 0) {
                assert->mUrlName = fileName;
            }
        }
        imgAssets_->clear();
    }
    FileDownload fileDownload;
    for (auto it = composition->mAssets.begin(); it != composition->mAssets.end(); ++it) {
        auto *asset = it->second;
        if (asset->mUrlName.empty()) {
            continue;
        }
        fileDownload.Start(asset->mUrlName, [asset](Rcp_Response *data) {
            if (asset == nullptr) {
                DRAWING_LOGE("DataManager::UpdataImageAssets: asset is nullptr!");
                return;
            }
            if (data->body.buffer == nullptr || data->body.length <= 0) {
                DRAWING_LOGE("DataManager::UpdataImageAssets: fileDownload empty!");
                return;
            }
            asset->loadImageData(std::string(data->body.buffer, data->body.length));
            asset->mUrlName.clear();
        });
    }
    fileDownload.Sync();
}

bool DataManager::InitFromJson(std::string json)
{
    json.erase(std::remove_if(json.begin(), json.end(), ::isspace), json.end()); // 去除前后空格
    HITRACE("Lottie::Format Json", FormatJson((uint8_t *)&json[0], json.size()));
    if (!json.empty() && json.front() == '{' && json.back() == '}') {
        path_ = GeneratorKey(json);
        std::string jsonData = std::move(json);
        auto composition = rlottie::Animation::loadCompositionFromData(std::move(jsonData), path_);
        UpdataImageAssets(composition);
        player = rlottie::Animation::loadAnimationFromComposition(composition);
        if (player == nullptr) {
            return false;
        }
        this->SetTotalFrames(static_cast<uint32_t>(player->totalFrame()));
        return true;
    }
    return false;
}

bool DataManager::InitFromFile(std::string path)
{
    path_ = path;
    std::unique_ptr<JsonData> ptr = std::make_unique<CommonParse>(path);
    ptr->SetUseCache(useCache_);
    ptr->Resolve();
    auto data = ptr->GetData();
    if (data.second == 0) {
        return false;
    }
    filePath_ = ptr->GetFilePath();
    HITRACE("Lottie::Format Json", FormatJson(data.first.get(), data.second));
    std::string jsonData(reinterpret_cast<char *>(data.first.get()), data.second);
    auto composition = rlottie::Animation::loadCompositionFromData(std::move(jsonData), path_, ptr->GetResourcePath());
    UpdataImageAssets(composition);
    player = rlottie::Animation::loadAnimationFromComposition(composition);
    if (player == nullptr) {
        return false;
    }
    this->SetTotalFrames(static_cast<uint32_t>(player->totalFrame()));
    return true;
}

bool DataManager::InitFromFile(Resource resource)
{
    path_ = resource.moduleName + "/" + resource.bundleName + "/" + resource.params;
    std::unique_ptr<JsonData> ptr = std::make_unique<ResourceParse>(resource, mNativeResMgr_);
    ptr->SetUseCache(useCache_);
    ptr->Resolve();
    auto data = ptr->GetData();
    if (data.second == 0) {
        return false;
    }
    filePath_ = ptr->GetFilePath();
    HITRACE("Lottie::Format Json", FormatJson(data.first.get(), data.second));
    std::string jsonData(reinterpret_cast<char *>(data.first.get()), data.second);
    auto composition = rlottie::Animation::loadCompositionFromData(std::move(jsonData), path_, ptr->GetResourcePath());
    UpdataImageAssets(composition);
    player = rlottie::Animation::loadAnimationFromComposition(composition);
    if (player == nullptr) {
        return false;
    }
    this->SetTotalFrames(static_cast<uint32_t>(player->totalFrame()));
    return true;
}

void DataManager::SetTotalFrames(const uint32_t &totalFrames)
{
    totalFrames_ = totalFrames;
}

void DataManager::GetSize(size_t &width, size_t &height)
{
    if (player == nullptr) {
        return;
    }
    player->size(width, height);
}

const std::string DataManager::GetFilePath() const
{
    return filePath_;
}

const DataManager::RenderStatus DataManager::GetCurrentStatus() const
{
    return status_;
}

const bool DataManager::GetVisible() const
{
    return visible_;
}

void DataManager::SetVisible(bool visible)
{
    visible_ = visible;
}

void DataManager::SetImageAssetDelegate(const ImageAssetDelegateType &imgAssets)
{
    imgAssets_ = imgAssets;
}

/*
 * 用于判断传进来的二进制流数的连续4字节数据是否为"ty"
 */
static bool IsDestData(uint8_t *data, uint64_t i)
{
    uint64_t firstByte = i++;
    uint64_t secondByte = i++;
    uint64_t thirdByte = i++;
    uint64_t forthByte = i;
    if (data[firstByte] == '"' && data[secondByte] == 't' && data[thirdByte] == 'y' && data[forthByte] == '"') {
        return true;
    }
    return false;
}

/*
 * 用于判断当前数据是否正确，正常数据是从data[i]到data[j],期间不会有{和}字符，如果有就不正常
 */
static bool IsCorrectData(uint8_t *data, uint64_t &i)
{
    bool ret = true;
    while (data[i] != ',') {
        if (data[i] == '}' || data[i] == '{') {
            ret = false;
            break;
        }
        i++;
    }
    return ret;
}

/*
 * 交换数据
 * parm ss 表示源数据起始位置， se表示源数据末尾位置， ds表示目标起始位置
 */
static void SwapData(uint8_t *data, uint64_t ss, uint64_t se, uint64_t ds)
{
    int i;
    int j;
    for (i = ss; i <= se; i++) {
        uint8_t tmp = data[i];
        for (j = i; j > ds + 1; j--) {
            data[j] = data[j - 1];
        }
        data[ds + 1] = tmp;
        ds++;
    }
}

/*
 * 遍历字节流，调整"ty"属性的位置,提放至所有同级属性的最前面
 */
static void AdjustPosionForTY(uint8_t *data, uint64_t length)
{
    // 同级目标所对应的同级左大括号，必然在栈顶
    const size_t stackDepth = 50;
    const size_t dstMaxSize = 4;
    uint64_t stack[stackDepth] = {0};
    int pointer = -1;

    if (length < dstMaxSize) {
        return;
    }

    for (uint64_t i = 0; i < length - dstMaxSize; i++) {
        if (data[i] == '{') {
            stack[++pointer] = i;
        } else if (data[i] == '}') {
            pointer--;
            if (pointer < 0) {
                pointer = -1;
            }
        } else if (IsDestData(data, i)) {
            uint64_t j = i + dstMaxSize - 1;
            if (!IsCorrectData(data, j)) {
                continue;
            }
            if (pointer >= 0) {
                SwapData(data, i, j, stack[pointer]);
            }
        }
    }
}

void DataManager::SetUseCache(bool useCache)
{
    useCache_ = useCache;
}
/*
 * 该函数用于格式化json的字节流，扫描数据,存在异常字段时，进行删除或位置调整
 * rlottie进行解析的入口是字节流，这里参入统一使用字节流来处理数据
 */
void DataManager::FormatJson(uint8_t *data, uint64_t length)
{
    AdjustPosionForTY(data, length);
}
} // namespace LottieTurbo