/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef LOTTIETURBO_LOTTIE_CONFIG_DATA_H
#define LOTTIETURBO_LOTTIE_CONFIG_DATA_H

#include "dataManager/data_parse.h"
#include <cstdint>
#include <string>

namespace LottieTurbo {

// 初始化动画资源播放时的整体帧范围
class AnimationSegment {
public:
    float initFrame{0.0};
    float endFrame{0.0};
};

struct ImageAssetDelegate {
    std::string refId;
    std::string fileName;
};

using ImageAssetDelegateType = std::shared_ptr<std::vector<ImageAssetDelegate>>;

class LottieConfig {
public:
    const std::string &GetModuleName() const;

    const std::string &GetLottieId() const;
    void SetLottieId(std::string &lottieId);

    const std::variant<bool, int> &GetLoop() const;
    void SetLoop(std::variant<bool, int> loop);

    const bool &GetAutoplay() const;
    void SetAutoplay(bool &autoplay);

    const std::string &GetContentMode() const;
    void SetContentMode(std::string &contentMode);

    const std::string &GetPath() const;
    void SetPath(std::string &path);

    const Resource &GetResource() const;
    void SetResource(Resource &resource);

    const std::string &GetAnimationData() const;
    void SetAnimationData(std::string &animationData);

    const AnimationSegment &GetInitialSegment() const;
    void SetInitialSegment(AnimationSegment &initialSegment);

    const std::vector<AnimationSegment> &GetSegments() const;
    void SetSegments(std::vector<AnimationSegment> &segments);

    const ImageAssetDelegateType &GetImageAssetDelegate() const;
    void SetImageAssetDelegate(ImageAssetDelegateType &imgAssets);

    const double &GetFrameRate() const;
    void SetFrameRate(double &frameRate);

    std::shared_ptr<NativeResourceManager> GetResManager();
    void SetResManager(std::shared_ptr<NativeResourceManager> resManager);

    const bool &GetUseCache() const;
    void SetUseCache(bool &useCache);
    
    const double &GetRadius() const;
    
    //TODO:lipei---------------------------------------------
    const int64_t &GetAnimator() const;

protected:
    const std::string moduleName_{"LottieConfigData"};
    std::string lottieId_{""};
    std::variant<bool, int> loop_{true};
    bool autoplay_{true};
    std::string contentMode_{"Contain"};
    std::string path_{""};
    Resource resource_;
    std::string animationData_{""};
    AnimationSegment initialSegment_;
    std::vector<AnimationSegment> segments_;
    ImageAssetDelegateType imgAssets_{std::make_shared<std::vector<ImageAssetDelegate>>()};
    double frameRate_{0.0};
    std::shared_ptr<NativeResourceManager> resManager_;
    bool useCache_{true};
    double radius_{0.0};
    
    /**
     * TODO:lipei---------------------------------------------
     * cangjie 端利用 @ohos.animator 和 c 交互，c 端 Animator 对象的指针值。
     */
    int64_t animator;
};

} // namespace LottieTurbo

#endif
