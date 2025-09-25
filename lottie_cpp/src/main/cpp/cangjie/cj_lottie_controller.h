/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef LOTTIE4CJ_CJ_LOTTIE_CONTROLLER_H
#define LOTTIE4CJ_CJ_LOTTIE_CONTROLLER_H

#include <string>

#include "hilog/log.h"
#include <native_window/external_window.h>

#include "cangjie/lottie_controller_bridge.h"
#include "cangjie/lottie_xcomponent_node.h"

namespace LottieTurbo {

class CJLottieController {
public:
    //explicit CJLottieController(std::shared_ptr<LottieControllerBridge> &lottieControllerBridge) : lottieControllerBridge(lottieControllerBridge) {}
    explicit CJLottieController(std::shared_ptr<LottieControllerBridge> lottieControllerBridge) : lottieControllerBridge(lottieControllerBridge) {}

    ~CJLottieController(){};

    static std::string ClassName() { return "CJLottieController"; }
    
    std::shared_ptr<LottieControllerBridge> getLottieControllerBridge();

    OHNativeWindow* getWindow();
    void setWindow(OHNativeWindow *window);
    
    uint64_t getSurfaceWidth();
    void setSurfaceWidth(uint64_t surfaceWidth);
    
    uint64_t getSurfaceHeight();
    void setSurfaceHeight(uint64_t surfaceHeight);
    
    std::string getLottieId();
    void setLottieId(std::string lottieId);
    
    std::shared_ptr<LottieXcomponentNode> getLottieXcomponentNode();
    void setLottieXcomponentNode(std::shared_ptr<LottieXcomponentNode> lottieXcomponentNode);

private:

    std::shared_ptr<LottieControllerBridge> lottieControllerBridge;
    std::mutex locker;
    
    OHNativeWindow *window = nullptr;

    //std::string xcomponentId;
    std::string lottieId_;
    uint64_t surfaceWidth;
    uint64_t surfaceHeight;
    
    std::shared_ptr<LottieXcomponentNode> lottieXcomponentNode_;
};

extern "C" { // cangjie [LottieController] ffi

//CJBytesArray cj_PAGViewController_uniqueID();

int64_t cj_LottieController_init();
void cj_LottieController_onSurfaceCreated(const int64_t jView, const int64_t surfaceId, uint64_t surfaceWidth, uint64_t surfaceHeight);
void cj_LottieController_onSurfaceChanged(const int64_t jView, const int64_t surfaceId, uint64_t surfaceWidth, uint64_t surfaceHeight);

//----------------------------------------------------------------------------------------------------------------

CJBytesArray cj_LottieController_getId(const int64_t ptr);
bool cj_LottieController_setId(const int64_t ptr, char *lottieId);
CJBoolStruct cj_LottieController_isLoaded(const int64_t ptr);
CJFloat64Struct cj_LottieController_get_currentFrame(const int64_t ptr);
void cj_LottieController_set_currentFrame(const int64_t ptr, double currentFrame);
CJFloat64Struct cj_LottieController_get_currentRawFrame(const int64_t ptr);
void cj_LottieController_set_currentRawFrame(const int64_t ptr, double currentRawFrame);
CJFloat64Struct cj_LottieController_get_firstFrame(const int64_t ptr);
CJFloat64Struct cj_LottieController_get_totalFrames(const int64_t ptr);
CJFloat64Struct cj_LottieController_get_frameRate(const int64_t ptr);
CJFloat64Struct cj_LottieController_get_frameMult(const int64_t ptr);
CJFloat64Struct cj_LottieController_get_playSpeed(const int64_t ptr);
CJInt64Struct cj_LottieController_get_playDirection(const int64_t ptr);
CJInt64Struct cj_LottieController_get_playCount(const int64_t ptr);
CJBoolStruct cj_LottieController_get_isPaused(const int64_t ptr);
CJBoolStruct cj_LottieController_get_autoplay(const int64_t ptr);
CJInt64Struct cj_LottieController_get_loop(const int64_t ptr);
CJFloat64Struct cj_LottieController_get_timeCompleted(const int64_t ptr);
CJInt64Struct cj_LottieController_get_segmentPos(const int64_t ptr);
CJBoolStruct cj_LottieController_get_isSubframeEnabled(const int64_t ptr);
CJAnimationSegmentArray cj_LottieController_get_segments(const int64_t ptr);


//int64_t cj_LottieController_init();

void cj_LottieController_play(const int64_t ptr);
void cj_LottieController_stop(const int64_t ptr);
void cj_LottieController_togglePause(const int64_t ptr);
void cj_LottieController_destroy(const int64_t ptr);
void cj_LottieController_pause(const int64_t ptr);
void cj_LottieController_goToAndStop(const int64_t ptr, double value, bool isFrame);
void cj_LottieController_goToAndPlay(const int64_t ptr, double value, bool isFrame);
void cj_LottieController_setSegment(const int64_t ptr, double start, double end);
void cj_LottieController_playSegments(const int64_t ptr, AnimationSegment segments[], const int64_t len, bool forceFlag, bool hasForceFlag);

void cj_LottieController_resetSegments(const int64_t ptr, bool forceFlag);
void cj_LottieController_setSpeed(const int64_t ptr, double speed);
void cj_LottieController_setDirection(const int64_t ptr, const int64_t direction);
bool cj_LottieController_setSubframe(const int64_t ptr, bool useSubFrames);
CJFloat64Struct cj_LottieController_getDuration(const int64_t ptr, bool inFrames);
void cj_LottieController_triggerEvent(const int64_t ptr, char *name);
void cj_LottieController_addEventListener(const int64_t ptr, char *name, CallbackFunc callback);
void cj_LottieController_removeEventListener(const int64_t ptr, char *name, CallbackFunc callback, bool hasCallbackFunc);

//void cj_LottieController_changeColor(const int64_t ptr, char *layer, double startColor[], const int64_t startColorLen, 
//                                     double endColor[], const int64_t endColorLen);
void cj_LottieController_changeColor(const int64_t ptr, char *layer, double *startColor, const int64_t startColorLen, 
                                     double *endColor, const int64_t endColorLen);
void cj_LottieController_setContentMode(const int64_t ptr, char *contentMode);
void cj_LottieController_setFrameRate(const int64_t ptr, double frameRate);
void cj_LottieController_reload(const int64_t ptr, LottieDataConfig config);

void cj_LottieController_release(const int64_t ptr);


};

};


#endif //LOTTIE4CJ_CJ_LOTTIE_CONTROLLER_H
