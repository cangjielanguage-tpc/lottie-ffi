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
#include "dataManager/lottie_config_data.h"
#include <cstdint>

namespace LottieTurbo {

const std::string &LottieConfig::GetModuleName() const
{
    return moduleName_;
}

const std::string &LottieConfig::GetLottieId() const
{
    return lottieId_;
}

void LottieConfig::SetLottieId(std::string &lottieId)
{
    lottieId_ = lottieId;
}

const std::variant<bool, int> &LottieConfig::GetLoop() const
{
    return loop_;
}

void LottieConfig::SetLoop(std::variant<bool, int> loop)
{
    loop_ = loop;
}

const bool &LottieConfig::GetAutoplay() const
{
    return autoplay_;
}

void LottieConfig::SetAutoplay(bool &autoplay)
{
    autoplay_ = autoplay;
}

const std::string &LottieConfig::GetContentMode() const
{
    return contentMode_;
}

void LottieConfig::SetContentMode(std::string &contentMode)
{
    contentMode_ = contentMode;
}

const std::string &LottieConfig::GetPath() const
{
    return path_;
}

void LottieConfig::SetPath(std::string &path)
{
    path_ = path;
}

const Resource &LottieConfig::GetResource() const
{
    return resource_;
}

void LottieConfig::SetResource(Resource &resource)
{
    resource_ = resource;
}

const std::string &LottieConfig::GetAnimationData() const
{
    return animationData_;
}

void LottieConfig::SetAnimationData(std::string &animationData)
{
    animationData_ = animationData;
}

const AnimationSegment &LottieConfig::GetInitialSegment() const
{
    return initialSegment_;
}

void LottieConfig::SetInitialSegment(AnimationSegment &initialSegment)
{
    initialSegment_ = initialSegment;
}

const std::vector<AnimationSegment> &LottieConfig::GetSegments() const
{
    return segments_;
}

void LottieConfig::SetSegments(std::vector<AnimationSegment> &segments)
{
    segments_ = segments;
}

const ImageAssetDelegateType &LottieConfig::GetImageAssetDelegate() const
{
    return imgAssets_;
}

void LottieConfig::SetImageAssetDelegate(ImageAssetDelegateType &imgAssets)
{
    imgAssets_ = imgAssets;
}

const double &LottieConfig::GetFrameRate() const
{
    return frameRate_;
}

void LottieConfig::SetFrameRate(double &frameRate)
{
    frameRate_ = frameRate;
}

std::shared_ptr<NativeResourceManager> LottieConfig::GetResManager()
{
    return resManager_;
}

void LottieConfig::SetResManager(std::shared_ptr<NativeResourceManager> resManager)
{
    resManager_ = resManager;
}

const bool &LottieConfig::GetUseCache() const
{
    return useCache_;
}

void LottieConfig::SetUseCache(bool &useCache)
{
    useCache_ = useCache;
}

const double &LottieConfig::GetRadius() const
{
    return radius_;
}

//TODO:lipei---------------------------------------------
const int64_t &LottieConfig::GetAnimator() const
{
    return animator;
}

} // namespace LottieTurbo
