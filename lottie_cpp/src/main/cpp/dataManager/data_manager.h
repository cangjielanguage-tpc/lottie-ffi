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

#ifndef LOTTIETURBO_DATAMANAGER_H
#define LOTTIETURBO_DATAMANAGER_H

#include "dataManager/data_parse.h"
#include "dataManager/lottie_config_data.h"
#include "rlottie.h"
#include <cstdint>
#include <rawfile/raw_dir.h>
#include <rawfile/raw_file.h>
#include <rawfile/raw_file_manager.h>
#include <string>

namespace LottieTurbo {

class DataManager {
public:
    enum class RenderStatus { IDLE, RENDERING };
    ~DataManager();
    std::shared_ptr<rlottie::Animation> player{nullptr};
    std::shared_ptr<NativeResourceManager> mNativeResMgr_{nullptr};

    /**
     * @brief 解析lottie动画
     * @param path 文件路径
     */
    bool InitFromFile(std::string path);

    /**
     * @brief 解析lottie动画
     * @param resource 资源文件
     */
    bool InitFromFile(Resource resource);

    /**
     * @brief 解析lottie动画
     * @param json json文本
     */
    bool InitFromJson(std::string json);

    /**
     * @brief 设定动画宽高
     * @param width 动画宽度
     * @param height 动画高度
     */
    void InitRect(int32_t width, int32_t height);

    /**
     * @brief 设置总帧数
     * @param totalFrames 总帧数
     */
    void SetTotalFrames(const uint32_t &totalFrames);

    /**
     * @brief 获取动画文件宽高
     * @param width 宽度
     * @param height 高度
     */
    void GetSize(size_t &width, size_t &height);

    /**
     * @brief 获取动画文件路径
     */
    const std::string GetFilePath() const;

    /**
     * @brief 获取当前渲染状态
     */
    const RenderStatus GetCurrentStatus() const;

    /**
     * @brief 设置动画资源替换
     * @param imgAssets 动画资源内容与refId
     */
    void SetImageAssetDelegate(const ImageAssetDelegateType &imgAssets);

    /**
     * @brief 格式化json的字节流
     * @param data 文件数据
     * @param length 文件长度
     */
    void FormatJson(uint8_t *data, uint64_t length);

    /**
     * @brief 设置是否使用缓存
     * @param useCache true时使用缓存
     */
    void SetUseCache(bool useCache);

    /**
     * @brief 更新动画资源
     * @param composition rlottie解析json后的数据对象
     */
    void UpdataImageAssets(std::shared_ptr<rlottie::internal::model::Composition> &composition);

    /**
     * @brief 获取当前是否可见
     */
    const bool GetVisible() const;

    /**
     * @brief 设置当前是否可见
     */
    void SetVisible(bool visible);

private:
    std::string path_{""};
    int32_t width_{0};
    int32_t height_{0};
    uint32_t totalFrames_{0};
    std::string filePath_{""};
    std::atomic<RenderStatus> status_{RenderStatus::IDLE};
    ImageAssetDelegateType imgAssets_;
    bool useCache_{true};
    bool visible_{true};
};

} // namespace LottieTurbo
#endif // LOTTIE_C_DATAMANAGER_H
