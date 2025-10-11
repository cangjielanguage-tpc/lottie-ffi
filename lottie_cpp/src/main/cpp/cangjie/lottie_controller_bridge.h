/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef RLOTTIE510828_LOTTIE_CONTROLLER_BRIDGE_H
#define RLOTTIE510828_LOTTIE_CONTROLLER_BRIDGE_H


#include "common/common.h"
#include "controller/lottie_handler.h"
#include "dataManager/lottie_config_data.h"
#include <arkui/native_node_napi.h>
#include <string>

#include "cangjie/lottie_config_data_bridge.h"
#include "cangjie/lottie_data_config.h"
#include "cangjie/lottie_utils.h"

namespace LottieTurbo {
extern "C" { // cangjie [LottieController] ffi

typedef void (*CallbackFunc)();

enum class LottieBaseAction {
    PLAY,
    PAUSE,
    STOP,
    TOGGLE_PAUSE
};

class LottieControllerBridge {
public:
    LottieControllerBridge() = default;

    void HandleLottieBaseAction(LottieBaseAction action);

    void SetId(char *lottieId);
    CJBytesArray GetId();
    CJFloat64Struct GetFirstFrame();
    CJBoolStruct GetLoaded();
    void SetCurrentFrame(float currentFrame);
    CJFloat64Struct GetCurrentFrame();
    void SetCurrentRawFrame(float currentRawFrame);
    CJFloat64Struct GetCurrentRawFrame();
    CJFloat64Struct GetTotalFrames();
    CJFloat64Struct GetDataFrameRate();
    CJFloat64Struct GetFrameMult();
    CJInt64Struct GetPlayCount();
    CJBoolStruct GetIsPaused();
    CJBoolStruct GetAutoPlay();
    CJInt64Struct GetLoop();
    CJFloat64Struct GetTimeCompleted();
    CJInt64Struct GetSegmentPos();
    CJBoolStruct GetIsSubframeEnabled();

    CJFloat64Struct GetPlaySpeed();

    CJInt64Struct GetPlayDirection();
    CJAnimationSegmentArray GetSegments();

    void Play(); //static void Play();
    void Pause();
    void Stop();
    void TogglePause();

    void Destroy();
    void GoToAndStop(double value, bool isFrame);
    void GoToAndPlay(double value, bool isFrame);
    void SetSegment(double start, double end);
    void ResetSegments(bool forceFlag);
    void SetSpeed(double speed);
    void SetDirection(int direction);
    void PlaySegments(AnimationSegment segments[], int len, bool forceFlag, bool hasForceFlag);
    bool SetSubframe(bool useSubFrames);
    CJFloat64Struct GetDuration(bool inFrames);
    void TriggerEvent(char *name);
    void AddEventListener(char *name, CallbackFunc callback);
    void RemoveEventListener(char *name, CallbackFunc callback, bool hasCallbackFunc);
    void ChangeColor(char *layer, double *startColor, int startColorLen, double *endColor, int endColorLen);
    void ReLoad(LottieDataConfig config);
    void SetContentMode(char *contentMode);
    void SetFrameRate(double frameRate);

private:
    std::mutex locker;
    std::string lottieId_ = "";
    void UpdateLottieConfig(LottieDataConfig config,
                                   std::shared_ptr<LottieConfigBridge>& configData,
                                   std::shared_ptr<LottieHandler>& lottieHandler);

public:
    void SetLottieId(std::string lottieId);
};

};

};

#endif //RLOTTIE510828_LOTTIE_CONTROLLER_BRIDGE_H
