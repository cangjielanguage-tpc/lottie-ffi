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

#include "controller/lottie_handler.h"
#include "common/common.h"
#include "common/number_constant.h"
#include "dataManager/data_parse.h"
#include <cstdint>

namespace LottieTurbo {

void LottieHandler::AdvanceFrame()
{
    const float rate = 1000.0;
    // 因为每一帧的绘制时长不一致，所以每次进入此函数的时间间隔会有不同。需要计算实际的帧刷新时间间隔，从而得出应该刷新到哪一帧
    auto currentTimestamp = std::chrono::high_resolution_clock::now();
    double elapsedTime = rate / frameRate_; // elapsedTime默认值
    if (lastTimestamp_ != std::chrono::time_point<std::chrono::high_resolution_clock>()) {
        std::chrono::duration<double> duration = currentTimestamp - lastTimestamp_;
        elapsedTime = duration.count() * rate; // 计算帧刷新时间间隔ms
    }
    this->UpdateFrameModifier();
    this->AdvanceTime(elapsedTime);

    DRAWING_LOGD("AdvanceFrame(), absoluteFrameToPlay_: %{public}f", absoluteFrameToPlay_);
    DRAWING_LOGD("AdvanceFrame(), firstFrame: "
                 "%{public}f, endFrame: %{public}f, totalFrame: "
                 "%{public}f, offsetFrame: %{public}f",
                 firstFrame_, endFrame_, totalFrames_, offsetFrame_);
    lastTimestamp_ = currentTimestamp;
}

void LottieHandler::AdvanceTime(double const &elapsedTime)
{
    // 计算下一个相对帧，下一个相对帧 = 当前相对帧 + frameModifier_ * 本次应该跳过的帧数
    // 本次应该跳过的帧数 = 每毫秒应该绘制多少帧 * elapsedTime(上一次帧刷新到现在过去的时间ms)
    // frameModifier_ = 步长（根据速度和方向得出）* 每毫秒应该绘制多少帧
    // 每毫秒应该绘制多少帧 = 动画源帧率 / 1000.0
    auto nextFrame = this->AbsoluteFrameToRelative(absoluteFrameToPlay_) + frameModifier_ * elapsedTime;
    DRAWING_LOGD("AdvanceTime(), nextRelativeFrame: %{public}f, elapsedTime: %{public}f", nextFrame, elapsedTime);

    auto frame = nextFrame;
    if (nextFrame >= totalFrames_ || nextFrame < 0) {
        // 最后一帧时，检查是否需要替换segments_
        if (!forceFlag_ && segmentsCache_.size() > 0) {
            segments_ = segmentsCache_;
            segmentPos_ = 0;
            segmentsCache_.clear();
        }
        if (nextFrame < 0) {
            frame = -nextFrame + totalFrames_;
        }
        bool needsLoop = true;
        while (totalFrames_ && frame >= totalFrames_ && needsLoop) {
            frame -= totalFrames_;
            needsLoop = this->CheckLoop();
        }
        // 如果不需要loop了，停止在最后一帧
        if (!needsLoop) {
            frame = totalFrames_;
        }
        nextFrame = GetNextFrameToPlay(frame);
    }
    this->UpdateAbsoluteFrameToPlay(nextFrame);
}

void LottieHandler::ResetSegments(bool const &forceFlag)
{
    segments_.clear();
    segments_.push_back({.initFrame = 0, .endFrame = lottieFileTotalFrames_});
    if (forceFlag) {
        auto nextFrame = this->CheckSegmentUpdate();
        this->UpdateAbsoluteFrameToPlay(nextFrame);
    }
};

void LottieHandler::UpdateAbsoluteFrameToPlay(float const &relativeFrame)
{
    auto nextFrame = relativeFrame;
    // 取模运算，将nextFrame限制在totalFrames_的范围内
    if (nextFrame != totalFrames_) {
        nextFrame = std::fmod(nextFrame, totalFrames_);
    }
    if (nextFrame > totalFrames_) {
        DRAWING_LOGW("nextFrame is greater than totalFrames, nextFrame: %{public}f, totalFrames: %{public}f",
                     nextFrame, totalFrames_);
        return;
    }
    if (nextFrame < 0) {
        nextFrame += totalFrames_;
    }
    absoluteFrameToPlay_ = RelativeFrameToAbsolute(nextFrame);
    currentFrame_ = absoluteFrameToPlay_;
}

void LottieHandler::UpdateFrameModifier()
{
    Direction direction = direction_;
    const float rate = 1000.0;
    if (firstFrame_ > endFrame_) {
        direction = direction_ == Direction::FORWARD ? Direction::REVERSE : Direction::FORWARD;
    }
    // 前进步长 = 速度 * 方向
    float modifier = playSpeed_ * static_cast<int>(direction);
    // 每毫秒应该绘制多少帧 = 动画源帧率 / 1000.0
    // frameModifier_ = 前进步长 * 每毫秒应该绘制多少帧
    frameModifier_ = modifier * (lottieFileFrameRate_ / rate);
    DRAWING_LOGD("UpdateFrameModifier(), frameModifier_: %{public}f, direction: %{public}d, frameRate: %{public}f, "
                 "speed_: %{public}f,",
                 frameModifier_, static_cast<int>(direction), frameRate_, playSpeed_);
}

float LottieHandler::UpdateCurrentSegment(AnimationSegment const &segment)
{
    float nextFrame = INVALID_FRAME;
    auto initFrame = segment.initFrame;
    auto endFrame = segment.endFrame;
    DRAWING_LOGD("UpdateCurrentSegment(), initFrame: %{public}f, endFrame: %{public}f", initFrame, endFrame);
    if (initFrame == endFrame) {
        return nextFrame;
    }
    if (initFrame < endFrame) {
        totalFrames_ = endFrame - initFrame;
        offsetFrame_ = initFrame;
    } else {
        totalFrames_ = initFrame - endFrame;
        offsetFrame_ = endFrame + 1;
    }
    timeCompleted_ = totalFrames_;
    firstFrame_ = initFrame;
    endFrame_ = endFrame;

    this->UpdateFrameModifier();
    nextFrame = this->GetNextFrameToPlay(0);
    if (lottieListener_ && safeFunc_) {
        auto func = [](LottieListener *listener, LottieEventType type) {
            if (listener) {
                listener->TriggerEvent(type);
            }
        };
        auto trigger = new LottieHandler::TriggerData;
        trigger->func = std::bind(func, lottieListener_.get(), LottieEventType::ON_SEGMENT_START);

        safeFunc_(trigger);
    }
    return nextFrame;
}

float LottieHandler::CheckSegmentUpdate()
{
    float nextFrame = INVALID_FRAME;
    int curSegmentPos = -1;
    auto loopValue = lottieConfig_->GetLoop();
    while (segments_.size()) {
        if ((std::holds_alternative<bool>(loopValue) && std::get<bool>(loopValue)) ||
            (std::holds_alternative<int>(loopValue) && playCount_ < std::get<int>(loopValue))) {
            // 如果设置了循环，这里需要片段循环
            curSegmentPos = (segmentPos_ + 1) % segments_.size();
            // 如果超过最后一个片段，说明循环一圈了，这里循环计数加一
            if (segmentPos_ + 1 >= segments_.size()) {
                playCount_ += 1;
            }
        } else {
            // 如果没有设置循环，这里片段正常累加
            curSegmentPos = segmentPos_ + 1;
        }

        // 片段超出实际值，直接退出,不需要更新，停留再最后一个片段
        if (curSegmentPos >= segments_.size()) {
            break;
        }
        nextFrame = this->UpdateCurrentSegment(segments_[curSegmentPos]);
        // 跳过空片段
        if (nextFrame != INVALID_FRAME) {
            segmentPos_ = curSegmentPos;
            break;
        }
    }
    return nextFrame;
}

bool LottieHandler::CheckLoop()
{
    if (this->CheckSegmentUpdate() != INVALID_FRAME) {
        return true;
    }
    playCount_ += 1;
    auto loopValue = lottieConfig_->GetLoop();
    auto func = [](LottieListener *listener, LottieEventType type) {
        if (listener) {
            listener->TriggerEvent(type);
        }
    };
    if (std::holds_alternative<bool>(loopValue) && std::get<bool>(loopValue)) {
        auto trigger = new LottieHandler::TriggerData;
        trigger->func = std::bind(func, lottieListener_.get(), LottieEventType::ON_LOOP_COMPLETE);

        safeFunc_(trigger);
        return true;
    } else if (std::holds_alternative<int>(loopValue) && playCount_ < std::get<int>(loopValue)) {
        auto trigger = new LottieHandler::TriggerData;
        trigger->func = std::bind(func, lottieListener_.get(), LottieEventType::ON_LOOP_COMPLETE);

        safeFunc_(trigger);
        return true;
    } else {
        // 播放完成. 停留在最后一个segment上
        this->Complete();
    }
    return false;
}

float LottieHandler::GetNextFrameToPlay(float const &frame)
{
    auto nextFrame = frame;
    if (nextFrame < 0) {
        nextFrame = -nextFrame;
    }
    // 判断播放方向和segment的顺序关系
    if ((direction_ == Direction::FORWARD && firstFrame_ > endFrame_) || // 正向播放，逆序segment
        (direction_ == Direction::REVERSE && firstFrame_ < endFrame_)) { // 逆向播放，正序segment
        // 逆序播放时，从最大值作为起点
        nextFrame = totalFrames_ - nextFrame;
    }
    return nextFrame;
}

float LottieHandler::RelativeFrameToAbsolute(float const &relativeFrame)
{
    return relativeFrame + offsetFrame_;
}

float LottieHandler::AbsoluteFrameToRelative(float const &absoluteFrame)
{
    return absoluteFrame - offsetFrame_;
}

void LottieHandler::ResetLastTimeStamp()
{
    lastTimestamp_ = std::chrono::time_point<std::chrono::high_resolution_clock>();
}

void LottieHandler::Play()
{
    if (isPaused_) {
        isPaused_ = false;
        this->ResetLastTimeStamp();
        animator_->Play();
        playState_ = PlayState::PLAY;
    }
}

void LottieHandler::Pause()
{
    if (!isPaused_) {
        isPaused_ = true;
        animator_->Pause();
        if (playState_ != PlayState::INVISIBLE_PAUSE) {
            playState_ = PlayState::PAUSE;
        }
        this->ResetLastTimeStamp();
        if (renderCurrentFrame) {
            // 停止后需要渲染下一帧，否则不会刷新
            renderCurrentFrame();
        }
    }
}

void LottieHandler::Stop()
{
    // 重置下一帧为第0帧相对帧
    auto nextFrame = this->GetNextFrameToPlay(0);
    this->UpdateAbsoluteFrameToPlay(nextFrame);
    playState_ = PlayState::STOP;
    this->Complete();
    if (renderCurrentFrame) {
        // 停止后需要渲染下一帧，否则不会刷新
        renderCurrentFrame();
    }
}

LottieHandler::PlayState LottieHandler::GetPlayState() const
{
    return playState_;
}

void LottieHandler::SetPlayState(PlayState state)
{
    playState_ = state;
}

void LottieHandler::TogglePause()
{
    if (isPaused_) {
        this->Play();
    } else {
        this->Pause();
    }
}

void LottieHandler::GoToAndStop(float const &framesOrMs, std::optional<bool> &isFrame)
{
    bool isFrameFlag = isFrame.value_or(false);
    float nextFrame = 0;
    if (isFrameFlag) {
        nextFrame = this->GetNextFrameToPlay(framesOrMs);
    } else {
        nextFrame = this->GetNextFrameToPlay(frameModifier_ * framesOrMs);
    }
    this->UpdateAbsoluteFrameToPlay(nextFrame);
    if (this->renderCurrentFrame != nullptr) {
        this->renderCurrentFrame();
    }
    this->Pause();
}

void LottieHandler::GoToAndPlay(float const &framesOrMs, std::optional<bool> &isFrame)
{
    this->GoToAndStop(framesOrMs, isFrame);
    this->Play();
}

void LottieHandler::Complete()
{
    DRAWING_LOGD("Complete(), playCount: %{public}d", playCount_);
    this->Pause();
    playCount_ = 0;
    if (lottieListener_) {
        auto func = [](LottieListener *listener, LottieEventType type) {
            if (listener) {
                listener->TriggerEvent(type);
            }
        };
        auto trigger = new LottieHandler::TriggerData;
        trigger->func = std::bind(func, lottieListener_.get(), LottieEventType::ON_COMPLETE);

        safeFunc_(trigger);
    }
}

void LottieHandler::Reload()
{
    Stop();
    bool isLoadSuccess{true};

    LottieEventType type;
    if (lottieConfig_->GetAnimationData() != "") {
        isLoadSuccess = dataManager_->InitFromJson(lottieConfig_->GetAnimationData());
    } else if (lottieConfig_->GetPath() != "") {
        isLoadSuccess = dataManager_->InitFromFile(lottieConfig_->GetPath());
    } else {
        isLoadSuccess = dataManager_->InitFromFile(lottieConfig_->GetResource());
    }
    // 数据加载失败，包含json文件解析及其图片数据获取失败 或者成功type
    type = isLoadSuccess ? LottieEventType::ON_LOADED_IMAGES : LottieEventType::ON_DATA_FAILED;
    if (lottieListener_) {
        auto func = [](LottieListener *listener, LottieEventType type) {
            if (listener) {
                listener->TriggerEvent(type);
            }
        };
        auto trigger = new LottieHandler::TriggerData;
        trigger->func = std::bind(func, lottieListener_.get(), type);

        safeFunc_(trigger);
    }

    if (dataManager_->player) {
        // 从0开始，总帧数需减1
        lottieFileTotalFrames_ = static_cast<float>(dataManager_->player->totalFrame() - 1);
        lottieFileFrameRate_ = static_cast<float>(dataManager_->player->frameRate());
        frameRate_ = dataManager_->player->frameRate();
        if (lottieConfig_->GetInitialSegment().endFrame != 0) {
            UpdateCurrentSegment(lottieConfig_->GetInitialSegment());
        } else {
            UpdateCurrentSegment({.initFrame = 0, .endFrame = lottieFileTotalFrames_});
        }
        DRAWING_LOGD("LottieHandle::Reload, frameRate_: %{public}f", frameRate_);
        ArkUI_ExpectedFrameRateRange expectFrameRateRange = {.min = static_cast<uint32_t>(frameRate_),
                                                             .max = static_cast<uint32_t>(frameRate_),
                                                             .expected = static_cast<uint32_t>(frameRate_)};
        animator_->SetFrameRateRange(expectFrameRateRange);
        animator_->UpdateFrameRateRange();
        isLoaded_ = true;
        AdvanceFrame();
    } else {
        DRAWING_LOGE("LottieHandler::InitHandleData lottieId(%{public}s): dataManager_->player is NULL",
                     lottieId_.c_str());
    }
}

void LottieHandler::ChangeColor(std::string path, std::vector<float> setColor)
{
    auto size = setColor.size();
    if (size < RGBNUM) {
        return;
    }
    setColor[NUM_0] = setColor[NUM_0] / UINT8_MAX;
    setColor[NUM_1] = setColor[NUM_1] / UINT8_MAX;
    setColor[NUM_2] = setColor[NUM_2] / UINT8_MAX;

    rlottie::Color color = {setColor[NUM_0], setColor[NUM_1], setColor[NUM_2]};
    if (!dataManager_->player) {
        return;
    }
    dataManager_->player->setValue<rlottie::Property::FillColor>(path, color);
    if (size == RGBANUM) {
        setColor[NUM_3] = setColor[NUM_3] * NUM_100;
        dataManager_->player->setValue<rlottie::Property::FillOpacity>(path, setColor[NUM_3]);
    }
}

void LottieHandler::ChangeColor(std::string path, std::vector<float> startColor, std::vector<float> endColor)
{
    if (startColor.size() < RGBNUM || endColor.size() < RGBNUM) {
        return;
    }
    startColor[NUM_0] = startColor[NUM_0] / UINT8_MAX;
    startColor[NUM_1] = startColor[NUM_1] / UINT8_MAX;
    startColor[NUM_2] = startColor[NUM_2] / UINT8_MAX;
    endColor[NUM_0] = endColor[NUM_0] / UINT8_MAX;
    endColor[NUM_1] = endColor[NUM_1] / UINT8_MAX;
    endColor[NUM_2] = endColor[NUM_2] / UINT8_MAX;

    rlottie::Color color = {startColor, endColor};
    if (!dataManager_->player) {
        return;
    }
    dataManager_->player->setValue<rlottie::Property::FillColor>(path, color);
    if (startColor.size() == RGBANUM && endColor.size() == RGBANUM) {
        startColor[NUM_3] = startColor[NUM_3] * NUM_100;
        endColor[NUM_3] = endColor[NUM_3] * NUM_100;
        rlottie::Opacity opacity = {startColor[NUM_3], endColor[NUM_3]};
        dataManager_->player->setValue<rlottie::Property::OpacityKey>(path, opacity);
    }
}

void LottieHandler::SetAnimator()
{
    animator_->Init();
}

void LottieHandler::SetRenderCallBack(std::function<void()> func)
{
    renderCurrentFrame = func;
}

void LottieHandler::SetAnimatorCallBack(std::function<void()> func)
{
    animator_->FunctionCallBack = func;
    renderAndAdvanceFrame = func;
}

void LottieHandler::TriggerListenerEvent(LottieEventType type)
{
    if (lottieListener_ && safeFunc_) {
        auto func = [](LottieListener *listener, LottieEventType type) {
            if (listener) {
                listener->TriggerEvent(type);
            }
        };
        auto trigger = new LottieHandler::TriggerData;
        trigger->func = std::bind(func, lottieListener_.get(), type);
        safeFunc_(trigger);
    }
}

bool LottieHandler::LoadAnimationData()
{
    bool isLoadSuccess{true};

    if (lottieConfig_->GetAnimationData() != "") {
        isLoadSuccess = dataManager_->InitFromJson(lottieConfig_->GetAnimationData());
    } else if (lottieConfig_->GetPath() != "") {
        isLoadSuccess = dataManager_->InitFromFile(lottieConfig_->GetPath());
    } else {
        isLoadSuccess = dataManager_->InitFromFile(lottieConfig_->GetResource());
    }

    // 触发数据加载事件(成功或失败)
    LottieEventType type = isLoadSuccess ? LottieEventType::ON_LOADED_IMAGES : LottieEventType::ON_DATA_FAILED;
    TriggerListenerEvent(type);

    return isLoadSuccess;
}

bool LottieHandler::InitializeAnimationParameters()
{
    if (!dataManager_->player) {
        DRAWING_LOGE("LottieHandler::InitializeAnimationParameters lottieId(%{public}s): dataManager_->player is NULL",
                     lottieId_.c_str());
        return false;
    }

    // 从0开始，总帧数需减1
    const int16_t rate = 1000;
    lottieFileTotalFrames_ = static_cast<float>(dataManager_->player->totalFrame() - 1);
    lottieFileFrameRate_ = static_cast<float>(dataManager_->player->frameRate());

    // 如果没有指定帧率，使用文件的帧率
    frameRate_ = (lottieConfig_->GetFrameRate() <= 0) ? lottieFileFrameRate_ : lottieConfig_->GetFrameRate();
    frameMult_ = frameRate_ / rate;

    // 设置动画段落
    if (lottieConfig_->GetInitialSegment().endFrame == 0) {
        UpdateCurrentSegment({.initFrame = 0, .endFrame = lottieFileTotalFrames_});
    } else {
        UpdateCurrentSegment(lottieConfig_->GetInitialSegment());
    }

    return true;
}

void LottieHandler::ConfigureFrameRate()
{
    DRAWING_LOGD("LottieHandler::ConfigureFrameRate(), frameRate_: %{public}f", frameRate_);
    ArkUI_ExpectedFrameRateRange expectFrameRateRange = {
        .min = static_cast<uint32_t>(frameRate_),
        .max = static_cast<uint32_t>(frameRate_),
        .expected = static_cast<uint32_t>(frameRate_)
    };
    animator_->SetFrameRateRange(expectFrameRateRange);
    animator_->UpdateFrameRateRange();
    isLoaded_ = true;
}

bool LottieHandler::InitHandleData(void)
{
    // 加载动画数据
    bool isLoadSuccess = LoadAnimationData();
    if (!isLoadSuccess) {
        return false;
    }

    // 初始化动画参数
    if (!InitializeAnimationParameters()) {
        return false;
    }

    // 配置帧率
    ConfigureFrameRate();

    // 触发数据就绪事件
    TriggerListenerEvent(LottieEventType::ON_DATA_READY);

    return true;
}

LottieHandler::LottieHandler(std::string lottieId, std::shared_ptr<LottieConfig> &&lottieConfig)
    : lottieId_(lottieId), lottieConfig_(lottieConfig)
{
    const float rate = 1000.0;
    const float defFrameRate = 60.0f;

    //直接在 cj 侧创建 @ohos.animator，不要调用 node->SetAnimator 在 c 侧创建新的 Animator。
    int64_t ptr = lottieConfig->GetAnimator();
    if (ptr <= 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieHandler::LottieHandler", "CJAnimator's pointer <= 0.");    
        return;
    }
    intptr_t viewPtrInt = static_cast<intptr_t>(ptr);
    CJAnimator *cjAnimator = reinterpret_cast<CJAnimator *>(viewPtrInt);
    if (!cjAnimator) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieHandler::LottieHandler", "CJAnimator is null.");
        return;
    }
    std::shared_ptr<Animator> animators = cjAnimator->getAnimator();
    if (!animators) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieHandler::LottieHandler", "Animator is null.");
        return;
    }
    animator_ = animators;

    dataManager_ = std::make_shared<DataManager>();
    dataManager_->mNativeResMgr_ = lottieConfig_->GetResManager();

    frameMult_ = lottieConfig->GetFrameRate() / rate;
    frameRate_ = lottieConfig->GetFrameRate() > 0 ? lottieConfig->GetFrameRate() : defFrameRate;
    segments_ = lottieConfig->GetSegments();
    dataManager_->SetImageAssetDelegate(lottieConfig->GetImageAssetDelegate());
    dataManager_->SetUseCache(lottieConfig_->GetUseCache());
}

LottieHandler::~LottieHandler()
{
    SetAnimatorCallBack(nullptr);
    SetRenderCallBack(nullptr);
    SetContentModeUpdateCallback(nullptr);
    animator_.reset();
    dataManager_.reset();
    if (lottieListener_ && safeFunc_) {
        auto func = [](LottieListener *listener, napi_threadsafe_function safeFunc) {
            if (listener) {
                delete listener;
                listener = nullptr;
            }
        };
        auto trigger = new LottieHandler::TriggerData;
        //TODO:lipei------------------[2250923]----------------------------------
        //safeFunc_(trigger);
    }
}

float const LottieHandler::GetCurrentFrame()
{
    auto currentFrame = AbsoluteFrameToRelative(absoluteFrameToPlay_);
    return isSubframeEnabled_ ? currentFrame : std::round(currentFrame);
}

float const LottieHandler::GetCurrentRawFrame()
{
    auto currentFrame = AbsoluteFrameToRelative(absoluteFrameToPlay_);
    return currentFrame;
}

float const LottieHandler::GetAbsoluteFrameToPlay()
{
    return absoluteFrameToPlay_;
}

float const LottieHandler::GetLottieFileTotalFrames()
{
    return lottieFileTotalFrames_;
}

float const LottieHandler::GetLottieFileTotalFrameRate()
{
    return lottieFileFrameRate_;
}

float const LottieHandler::GetFrameRate()
{
    return frameRate_;
}

void LottieHandler::SetCurrentFrame(float const &currenFrame)
{
    auto frameToPlay = this->GetNextFrameToPlay(currenFrame);
    auto absoluteFrame = this->RelativeFrameToAbsolute(frameToPlay);
    absoluteFrameToPlay_ = isSubframeEnabled_ ? absoluteFrame : static_cast<int>(absoluteFrame);
}

const std::string &LottieHandler::GetLottieId() const
{
    return this->lottieId_;
}
void LottieHandler::SetLottieSize(int const &width, int const &height)
{
    this->width_ = width;
    this->height_ = height;
}

LottieSize LottieHandler::GetLottieSize()
{
    LottieSize lottieSize;
    lottieSize.width = this->width_;
    lottieSize.height = this->height_;
    return lottieSize;
}

void LottieHandler::SetSegments(AnimationSegment const &segment)
{
    segmentPos_ = -1;
    segments_.clear();
    segments_.push_back(segment);
    this->UpdateCurrentSegment(segment);
}

void LottieHandler::SetSegments(std::vector<AnimationSegment> &&segments)
{
    segmentPos_ = -1;
    if (forceFlag_) {
        segments_.clear();
        segments_ = std::move(segments);
        auto nextFrame = this->CheckSegmentUpdate();
        this->UpdateAbsoluteFrameToPlay(nextFrame);
        return;
    }
    segmentsCache_ = segments;
}

void LottieHandler::SetFileFrameRate(float const &fileFrameRate)
{
    lottieFileFrameRate_ = fileFrameRate;
}

void LottieHandler::SetForceFlag(bool const &forceFlag)
{
    forceFlag_ = forceFlag;
}

void LottieHandler::SetDirection(int const &direction)
{
    auto value = direction < 0 ? Direction::REVERSE : Direction::FORWARD;
    // -1速度播放时设置反向播放，不变成正向播放
    if (direction_ != value) {
        direction_ = value;
        this->UpdateFrameModifier();
    }
}

void LottieHandler::SetSpeed(double const &speed)
{
    if (speed == 0) {
        this->Pause();
        return;
    }
    if (speed > 0) {
        SetDirection(1);
    } else {
        SetDirection(-1);
    }
    auto playSpeed = abs(speed);
    if (playSpeed_ != playSpeed) {
        playSpeed_ = playSpeed;
        this->UpdateFrameModifier();
    }
    if (isPaused_) {
        this->Play();
    }
}

void LottieHandler::SetFrameRate(double const &frameRate)
{
    const float rate = 1000.0;
    if (frameRate < 0) {
        return;
    }
    frameRate_ = frameRate;
    frameMult_ = frameRate / rate;
}

void LottieHandler::SetContentModeUpdateCallback(std::function<void()> func)
{
    updateContentMode = func;
}

float const LottieHandler::GetTotalFrames()
{
    return totalFrames_;
}

void LottieHandler::SetTotalFrames(const int &totalFrames)
{
    totalFrames_ = totalFrames;
}

bool const LottieHandler::GetIsLoaded()
{
    return isLoaded_;
}

float const LottieHandler::GetFirstFrame()
{
    return firstFrame_;
}

int const LottieHandler::GetPlayCount()
{
    return playCount_;
}

bool const LottieHandler::GetIsPaused()
{
    return isPaused_;
}

float const LottieHandler::GetIsSubframeEnabled()
{
    return isSubframeEnabled_;
}

float const LottieHandler::GetPlaySpeed()
{
    return playSpeed_;
}

int const LottieHandler::GetDirection()
{
    return static_cast<int>(direction_);
}

std::vector<AnimationSegment> const LottieHandler::GetSegments()
{
    if (segments_.size() == 0) {
        std::vector<AnimationSegment> segments;
        segments.push_back({.initFrame = firstFrame_, .endFrame = endFrame_});
        return segments;
    }
    return segments_;
}

void LottieHandler::SetIsSubframeEnabled(bool const &isSubframeEnabled)
{
    isSubframeEnabled_ = isSubframeEnabled;
}

float const LottieHandler::GetTimeCompleted()
{
    return timeCompleted_;
}

int const LottieHandler::GetSegmentPos()
{
    return ((segmentPos_ == -1) ? (0) : (segmentPos_));
}

std::shared_ptr<LottieConfig> LottieHandler::GetLottieConfig()
{
    return lottieConfig_;
}

} // namespace LottieTurbo