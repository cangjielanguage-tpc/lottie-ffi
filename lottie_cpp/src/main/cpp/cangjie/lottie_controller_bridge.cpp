/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "lottie_controller_bridge.h"

#include "cangjie/lottie_controller_bridge.h"
#include "common/number_constant.h"
#include "lottie_listener.h"
#include "node/lottie_manager.h"
#include "task_pool/task_pool.h"
#include <cstdint>
#include <map>


#include "hilog/log.h"

namespace LottieTurbo {
extern "C" { // cangjie [LottieController] ffi

//------------------------------------------------------------------------------------------

static const std::unordered_map<std::string, LottieEventType> listenerEventMap = {
    {"drawFrame", LottieEventType::ON_DRAW_FRAME},       {"enterFrame", LottieEventType::ON_ENTER_FRAME},
    {"loopComplete", LottieEventType::ON_LOOP_COMPLETE}, {"complete", LottieEventType::ON_COMPLETE},
    {"segmentStart", LottieEventType::ON_SEGMENT_START}, {"destroy", LottieEventType::ON_DESTROY},
    {"config_ready", LottieEventType::ON_CONFIG_READY},  {"data_ready", LottieEventType::ON_DATA_READY},
    {"DOMLoaded", LottieEventType::ON_DOM_LOADED},       {"error", LottieEventType::ON_ERROR},
    {"data_failed", LottieEventType::ON_DATA_FAILED},    {"loaded_images", LottieEventType::ON_LOADED_IMAGES}};

static const size_t EVENT_LISTENER_ARGC = 2;
static const size_t PLAY_SEGMENT_ARGC = 2;
static const size_t SEGMENT_SIZE = 2;
static const size_t GO_TO_ARGS_ONE_OPTIONAL = 2;
static const size_t GO_TO_ARGS_TWO_OPTIONAL = 3;

void LottieControllerBridge::HandleLottieBaseAction(LottieBaseAction action)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler == nullptr) {
        DRAWING_LOGE("LottieControllerBridge::HandleLottieBaseAction lottieId(%{public}s): "
                     "Failed to get LottieHandler",
                     this->lottieId_.c_str());
        return;
    }

    switch (action) {
        case LottieBaseAction::PLAY :
            DRAWING_LOGD("LottieControllerBridge::Play() called, id: %{public}s", this->lottieId_.c_str());
            lottieHandler->Play();
            break;
        case LottieBaseAction::PAUSE :
            DRAWING_LOGD("LottieControllerBridge::Pause() called, id: %{public}s", this->lottieId_.c_str());
            lottieHandler->Pause();
            break;
        case LottieBaseAction::STOP :
            DRAWING_LOGD("LottieControllerBridge::Stop() called, id: %{public}s", this->lottieId_.c_str());
            lottieHandler->Stop();
            break;
        case LottieBaseAction::TOGGLE_PAUSE :
            DRAWING_LOGD("LottieControllerBridge::TogglePause() called, id: %{public}s", this->lottieId_.c_str());
            lottieHandler->TogglePause();
            break;
        default :
            DRAWING_LOGE("LottieControllerBridge::HandleLottieBaseAction lottieId(%{public}s): "
                         "Invalid action requested in LottieControllerBridge::HandleLottieAction()",
                         this->lottieId_.c_str());
            return;
    }

    return;
}

void LottieControllerBridge::Play()
{
    //std::lock_guard lock_guard(locker);
    return LottieControllerBridge::HandleLottieBaseAction(LottieBaseAction::PLAY);
}

void LottieControllerBridge::Pause()
{
    //std::lock_guard lock_guard(locker);
    return LottieControllerBridge::HandleLottieBaseAction(LottieBaseAction::PAUSE);
}

void LottieControllerBridge::Stop()
{
    //std::lock_guard lock_guard(locker);
    return LottieControllerBridge::HandleLottieBaseAction(LottieBaseAction::STOP);
}

void LottieControllerBridge::TogglePause()
{
    //std::lock_guard lock_guard(locker);
    return LottieControllerBridge::HandleLottieBaseAction(LottieBaseAction::TOGGLE_PAUSE);
}

void LottieControllerBridge::Destroy()
{
    //std::lock_guard lock_guard(locker);
    auto handler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (handler == nullptr || handler->isDestroy_) {
        return;
    }
    handler->isDestroy_ = true;
    TaskPoolLocal::GetInstance().PushTask(
        [](std::string id) {
            auto handler = LottieManager::GetInstance().GetHandleById(id);
            if (handler == nullptr) {
                return;
            }
            auto func = [](std::string id) { LottieManager::GetInstance().DisposeLottieNode(id); };
            auto trigger = new LottieHandler::TriggerData;
            trigger->func = std::bind(func, id);

            handler->safeFunc_(trigger);
        },
        this->lottieId_);
    return;
}

void LottieControllerBridge::GoToAndStop(double value, bool isFrameNew)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    DRAWING_LOGD("LottieControllerBridge::GoToAndStop() called, id: %{public}s", this->lottieId_.c_str());
    if (lottieHandler == nullptr) {
        return;
    }
    std::optional<bool> isFrame = isFrameNew;
    lottieHandler->GoToAndStop(value, isFrame);

    return;
}

void LottieControllerBridge::GoToAndPlay(double value, bool isFrameNew)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    DRAWING_LOGD("LottieControllerBridge::GoToAndPlay() called, id: %{public}s", this->lottieId_.c_str());
    if (lottieHandler == nullptr) {
        return;
    }
    std::optional<bool> isFrame = isFrameNew;
    lottieHandler->GoToAndPlay(value, isFrame);

    return;
}

void LottieControllerBridge::SetSegment(double start, double end)
{
    //std::lock_guard lock_guard(locker);
    auto initFrame = start;
    auto endFrame = end;

    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    DRAWING_LOGD("LottieControllerBridge::SetSegment() called, id: %{public}s", this->lottieId_.c_str());
    if (lottieHandler != nullptr) {
        lottieHandler->SetSegments({static_cast<float>(initFrame), static_cast<float>(endFrame)});
    }
    return;
}

void LottieControllerBridge::PlaySegments(AnimationSegment segmentsNew[], int arrayLength, bool forceFlag, bool hasForceFlag)
{
    //std::lock_guard lock_guard(locker);
    std::vector<AnimationSegment> segments;
    for (int i = 0; i < arrayLength; i++) {
        auto segment = segmentsNew[i];
        segments.push_back({.initFrame = static_cast<float>(segment.initFrame), .endFrame = static_cast<float>(segment.endFrame)});
    }

    if (!segments.empty()) {
        auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
        DRAWING_LOGD("LottieControllerBridge::SetSegment() called, id: %{public}s", this->lottieId_.c_str());
        if (lottieHandler != nullptr) {
            if (hasForceFlag) {
                lottieHandler->SetForceFlag(forceFlag);
            }
            lottieHandler->SetSegments(std::move(segments));
            lottieHandler->Play();
        }
    }

    return;
}

void LottieControllerBridge::ResetSegments(bool forceFlag)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    DRAWING_LOGD("LottieControllerBridge::ResetSegments() called, id: %{public}s", this->lottieId_.c_str());
    if (lottieHandler != nullptr) {
        lottieHandler->ResetSegments(forceFlag);
    }
    return;
}

void LottieControllerBridge::SetSpeed(double speed)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    DRAWING_LOGD("LottieControllerBridge::SetSpeed() called, id: %{public}s", this->lottieId_.c_str());
    if (lottieHandler != nullptr) {
        lottieHandler->SetSpeed(speed);
    }
    return;
}

void LottieControllerBridge::SetDirection(int direction)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    DRAWING_LOGD("LottieControllerBridge::SetDirection() called, id: %{public}s", this->lottieId_.c_str());
    if (lottieHandler != nullptr) {
        lottieHandler->SetDirection(direction);
    }
    return;
}

bool LottieControllerBridge::SetSubframe(bool isSubframeEnabled)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        lottieHandler->SetIsSubframeEnabled(isSubframeEnabled);
        DRAWING_LOGD("SetSubframe %{public}d", isSubframeEnabled);
    } else {
        return false;
    }
    return true;
}

void LottieControllerBridge::TriggerEvent(char *name)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    auto eventName = name;
    auto it = listenerEventMap.find(eventName);
    if (it == listenerEventMap.end()) {
        return;
    }
    if (lottieHandler != nullptr && lottieHandler->lottieListener_ != nullptr) {
        lottieHandler->lottieListener_->TriggerEvent(it->second);
    }
    return;
}

void LottieControllerBridge::AddEventListener(char *eventName, CallbackFunc callback)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    DRAWING_LOGD("LottieControllerBridge::AddEventListener() called, id: %{public}s", this->lottieId_.c_str());
    if (lottieHandler != nullptr) {
        auto it = listenerEventMap.find(eventName);
        if (it == listenerEventMap.end()) {
            return;
        }
        lottieHandler->lottieListener_->RegisterCallback(callback, it->second);
    }
    return;
}

void LottieControllerBridge::RemoveEventListener(char *eventName, CallbackFunc callback, bool hasCallbackFunc)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    //DRAWING_LOGD("LottieControllerBridge::RemoveEventListener() called, id: %{public}s", this->lottieId_.c_str());
    if (lottieHandler != nullptr) {
        auto it = listenerEventMap.find(eventName);
        if (it == listenerEventMap.end()) {
            return;
        }
        lottieHandler->lottieListener_->UnRegisterCallback(it->second);
        // 如果没有回调，直接返回
        if (!hasCallbackFunc) {
            return;
        }
        //是要调用一下传入的函数么？？？
        callback();
    }
    return;
}

void LottieControllerBridge::ChangeColor(char *layer, double *startColor, int startColorLen, double *endColor, int endColorLen)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler == nullptr) {
        return;
    }
    auto layerName = layer;

    if (endColorLen == 0) {
        auto length = startColorLen;
        std::vector<float> color;
        for (int i = 0; i < length; i++) {
            auto num = startColor[i];
            color.push_back(num);
        }
        lottieHandler->ChangeColor(layerName, color);
    } else if (endColorLen > 0) {
        auto startLenth = startColorLen;
        auto endLenth = endColorLen;
        std::vector<float> startColor;
        std::vector<float> endColor;
        for (int i = NUM_0; i < startLenth; i++) {
            auto num = startColor[i];
            startColor.push_back(num);
        }
        for (int i = NUM_0; i < endLenth; i++) {
            auto num = endColor[i];
            endColor.push_back(num);
        }
        lottieHandler->ChangeColor(layerName, startColor, endColor);
    }
    return;
}


void LottieControllerBridge::UpdateLottieConfig(LottieDataConfig config,
                                                std::shared_ptr<LottieConfigBridge>& configData,
                                                std::shared_ptr<LottieHandler>& lottieHandler)
{
    //std::lock_guard lock_guard(locker);
    //处理 resManager 字段 -- 
    //cj 的 ResourceManager 对象传不到 c代码层，改成在 cj 直接处理资源，转为 json 字符串或者 Byte 数组？？？

    //处理 loop 字段
    /**
     * 动画播放结束后, 是否循环播放。默认值 -1, 值为 -1 时代表无限循环播放, 值 >=1 时为设置重复播放的次数
     * 定为 Int64 数据类型，< 0 表示无限循环播放；= 0 表示 不循环播放；> 0 为设置重复播放的次数。
     */
    std::variant<bool, int> loop{false};

    if (config.loop < 0) {
        loop = true;
    } else if (config.loop == 0) {
        loop = false;
    } else {//(config.loop > 0
        loop = (int)config.loop;
    }
    lottieHandler->GetLottieConfig()->SetLoop(loop);


    bool autoplay = config.autoplay;
    lottieHandler->GetLottieConfig()->SetAutoplay(autoplay);

    std::string path = config.path;
    //暂时不做 path 字段兼容 Resource 类型。

    if (!path.empty()) {
        lottieHandler->GetLottieConfig()->SetPath(path);
    }

    AnimationSegment initialSegment;

    if (config.initialSegment[0] != config.initialSegment[1]) {
        initialSegment.initFrame = config.initialSegment[0];
        initialSegment.endFrame = config.initialSegment[1];
        lottieHandler->GetLottieConfig()->SetInitialSegment(initialSegment);
    }


    std::string contentMode = config.contentMode;
    if (!contentMode.empty()) {
        lottieHandler->GetLottieConfig()->SetContentMode(contentMode);
    }

    double frameRate = config.frameRate;
    constexpr int maxFrameRate = 120;
    if (frameRate > 0 && frameRate <= maxFrameRate) {
        lottieHandler->SetFrameRate(frameRate);
    }

    std::string animationData = config.animationData;
    if (!animationData.empty() || animationData.length() != lottieHandler->GetLottieConfig()->GetAnimationData().length()) {
        lottieHandler->GetLottieConfig()->SetAnimationData(animationData);
    }

    auto &imgAssets = const_cast<ImageAssetDelegateType &>(lottieHandler->
                      GetLottieConfig()->GetImageAssetDelegate());
    imgAssets->clear();

    if (config.setImageAssetDelegateSize > 0 && lottieHandler->dataManager_ != nullptr) {
        for (int index = 0; index < config.setImageAssetDelegateSize; index++) {
            auto rst = config.setImageAssetDelegate[index];
            ImageAssetDelegate iad = ImageAssetDelegate();
            iad.refId = rst.refId;
            iad.fileName = rst.fileName;

            imgAssets->emplace_back(iad);  // 更高效的方式
        }

        lottieHandler->dataManager_->SetImageAssetDelegate(imgAssets);
    }
}



void LottieControllerBridge::ReLoad(LottieDataConfig config)
{
    //std::lock_guard lock_guard(locker);
    auto configData = std::make_shared<LottieConfigBridge>(config);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler == nullptr) {
        return;
    }
    // 更新配置
    UpdateLottieConfig(config, configData, lottieHandler);

    // 执行重载任务
    TaskPoolLocal::GetInstance().PushTask(
        [](std::shared_ptr<LottieHandler> handler) {
            if (handler->isDestroy_) {
                return;
            }
            handler->Reload();
            if (handler->updateContentMode) {
                handler->updateContentMode();
            }
            if (handler->GetLottieConfig()->GetAutoplay()) {
                handler->Play();
            } else if (handler->renderAndAdvanceFrame) {
                handler->renderAndAdvanceFrame();
            }
        },
        lottieHandler);

    return;
}

void LottieControllerBridge::SetContentMode(char *contentModeNew)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler == nullptr) {
        return;
    }
    std::string contentMode = contentModeNew;
    lottieHandler->GetLottieConfig()->SetContentMode(contentMode);
    if (lottieHandler->updateContentMode) {
        lottieHandler->updateContentMode();
    }
    if (lottieHandler->renderCurrentFrame) {
        lottieHandler->renderCurrentFrame();
    }
    return;
}

void LottieControllerBridge::SetFrameRate(double frameRateNew)
{
    //std::lock_guard lock_guard(locker);
    constexpr int frameRateLimits = 120;

    double frameRate = frameRateNew;

    if (this != nullptr) {
        auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
        if (lottieHandler != nullptr) {
            float maxFrameRate = lottieHandler->GetLottieFileTotalFrameRate();
            if (frameRate == 0 || (maxFrameRate < frameRate && frameRate < frameRateLimits) ||
                (maxFrameRate < frameRateLimits && frameRate >= frameRateLimits)) {
                frameRate = maxFrameRate;
            } else if (frameRate <= 0) {
                return;
            } else if (frameRate >= frameRateLimits && maxFrameRate >= frameRateLimits) {
                frameRate = frameRateLimits;
            }
            ArkUI_ExpectedFrameRateRange expectFrameRateRange = {.min = static_cast<uint32_t>(frameRate),
                                                                 .max = static_cast<uint32_t>(frameRate),
                                                                 .expected = static_cast<uint32_t>(frameRate)};
            lottieHandler->Pause();
            lottieHandler->SetFrameRate(frameRate);
            lottieHandler->animator_->SetFrameRateRange(expectFrameRateRange);
            lottieHandler->animator_->UpdateFrameRateRange();
            lottieHandler->Play();
        }
    }
    return;
}


//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------
// Set 和 Get 方法不应该给LottieControllerBridge设值或取值，而是从对应的C++实现类里面取


void LottieControllerBridge::SetId(char *lottieId)
{
    //std::lock_guard lock_guard(locker);
    this->lottieId_ = lottieId;
}

CJBytesArray LottieControllerBridge::GetId()
{
    //std::lock_guard lock_guard(locker);
    if (this != nullptr) {
        std::string str = this->lottieId_.c_str();
        CJBytesArray lottieId = stringToCJBytesArray(str);
        return lottieId;
    }
    return {size: 0};
}

CJBoolStruct LottieControllerBridge::GetLoaded()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        bool isLoaded = lottieHandler->GetIsLoaded();
        return {res: isLoaded, callResult: true};
    }

    return {callResult: false};
}

CJFloat64Struct LottieControllerBridge::GetFirstFrame()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        float firstFrame = lottieHandler->GetFirstFrame();
        return {res: firstFrame, callResult: true};
    }
    return {callResult: false};
}

void LottieControllerBridge::SetCurrentFrame(float currentFrame)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler == nullptr) {
        lottieHandler->SetCurrentFrame(currentFrame);
        //DRAWING_LOGD("SetCurrentFrame %{public}f", currentFrame);
    }

    return;
}

CJFloat64Struct LottieControllerBridge::GetCurrentFrame()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        // 目前不支持子帧插值，只考虑返回整数值
        float currentFrame = lottieHandler->GetCurrentFrame();
        return {res: currentFrame, callResult: true};
    }
    return {callResult: false};
}

void LottieControllerBridge::SetCurrentRawFrame(float currentRawFrame)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        lottieHandler->SetCurrentFrame(currentRawFrame);
        //DRAWING_LOGD("SetCurrentRawFrame %{public}d", currentRawFrame);
    }
    return;
}

CJFloat64Struct LottieControllerBridge::GetCurrentRawFrame()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        float currentRawFrame = lottieHandler->GetCurrentRawFrame();
        return {res: currentRawFrame, callResult: true};
    }
    return {callResult: false};
}

CJFloat64Struct LottieControllerBridge::GetTotalFrames()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        float totalFrames = lottieHandler->GetTotalFrames();
        return {res: totalFrames, callResult: true};
    }
    return {callResult: false};
}

CJFloat64Struct LottieControllerBridge::GetDataFrameRate()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        double frameRate = lottieHandler->GetFrameRate();
        return {res: frameRate, callResult: true};
    }
    return {callResult: false};
}

CJFloat64Struct LottieControllerBridge::GetFrameMult()
{
    //std::lock_guard lock_guard(locker);
    constexpr float persent = 1000.0;
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        double frameMult = lottieHandler->GetFrameRate() / persent;
        return {res: frameMult, callResult: true};
    }
    return {callResult: false};
}

CJInt64Struct LottieControllerBridge::GetPlayCount()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        int playCount = lottieHandler->GetPlayCount();
        return {res: playCount, callResult: true};
    }
    return {callResult: false};
}

CJBoolStruct LottieControllerBridge::GetIsPaused()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        bool isPaused = lottieHandler->GetIsPaused();
        return {res: isPaused, callResult: true};
    }
    return {callResult: false};
}

CJBoolStruct LottieControllerBridge::GetAutoPlay()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        bool autoplay = lottieHandler->GetLottieConfig()->GetAutoplay();
        return {res: autoplay, callResult: true};
    }
    return {callResult: false};
}

CJInt64Struct LottieControllerBridge::GetLoop()
{
    //std::lock_guard lock_guard(locker);
    /**
     * 动画播放结束后, 是否循环播放。默认值 -1, 值为 -1 时代表无限循环播放, 值 >=1 时为设置重复播放的次数
     * 定为 Int64 数据类型，< 0 表示无限循环播放；= 0 表示 不循环播放；> 0 为设置重复播放的次数。
     */
    auto loopNum = 0;
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        auto loopValue = lottieHandler->GetLottieConfig()->GetLoop();
        if (std::holds_alternative<bool>(loopValue)) {
            auto flag = std::get<bool>(loopValue);
            if (flag) {
                loopNum = -1;
            }
        } else if (std::holds_alternative<int>(loopValue)) {
            loopNum = std::get<int>(loopValue);
        }
    } else {
        return {callResult: false};
    }
    return {res: loopNum, callResult: true};
}

CJFloat64Struct LottieControllerBridge::GetTimeCompleted()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        float timeCompleted = lottieHandler->GetTimeCompleted();
        return {res: timeCompleted, callResult: true};
    }
    return {callResult: false};
}

CJInt64Struct LottieControllerBridge::GetSegmentPos()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        int segmentPos = static_cast<int32_t>(lottieHandler->GetSegmentPos());
        return {res: segmentPos, callResult: true};
    }
    return {callResult: false};
}

CJBoolStruct LottieControllerBridge::GetIsSubframeEnabled()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        bool isSubframeEnabled = lottieHandler->GetIsSubframeEnabled();
        return {res: isSubframeEnabled, callResult: true};
    }
    return {callResult: false};
}

CJFloat64Struct LottieControllerBridge::GetDuration(bool inFrames)
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        double duration = inFrames ? lottieHandler->GetTotalFrames()
                                  : lottieHandler->GetTotalFrames() / lottieHandler->GetLottieFileTotalFrameRate();
        return {res: duration, callResult: true};
    }
    return {callResult: false};
}

CJFloat64Struct LottieControllerBridge::GetPlaySpeed()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        double playSpeed = lottieHandler->GetPlaySpeed();
        return {res: playSpeed, callResult: true};
    }
    return {callResult: false};
}

CJInt64Struct LottieControllerBridge::GetPlayDirection()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler != nullptr) {
        long long direction = static_cast<int32_t>(lottieHandler->GetDirection());
        return {res: direction, callResult: true};
    }
    //动画播放方向，1 正向播放；-1 反向播放，0 获取播放方向失败。
    return {callResult: false};
}

CJAnimationSegmentArray LottieControllerBridge::GetSegments()
{
    //std::lock_guard lock_guard(locker);
    auto lottieHandler = LottieManager::GetInstance().GetHandleById(this->lottieId_);
    if (lottieHandler == nullptr) {
        return {size: 0};
    }
    auto segments = lottieHandler->GetSegments(); //std::vector<AnimationSegment>
    int64_t len = segments.size();
    AnimationSegment *result = new AnimationSegment[len];
    for (size_t i = 0; i < len; i++) {
        auto segment = segments[i];
        AnimationSegment element = AnimationSegment();
        element.initFrame = segment.initFrame;
        element.endFrame = segment.endFrame;
        result[i] = element;
    }
    return CJAnimationSegmentArray{len, result};
}

void LottieControllerBridge::SetLottieId(std::string lottieId)
{
    //std::lock_guard lock_guard(locker);
    this->lottieId_ = lottieId;
}

};
};
