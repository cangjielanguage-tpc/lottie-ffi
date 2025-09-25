/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "cj_lottie_controller.h"

namespace LottieTurbo {

extern "C" { // cangjie [LottieController] ffi

std::shared_ptr<LottieControllerBridge> CJLottieController::getLottieControllerBridge()
{
    std::lock_guard lock_guard(locker);
    return lottieControllerBridge;
}

OHNativeWindow* CJLottieController::getWindow()
{
    std::lock_guard lock_guard(locker);
    return window;
}
void CJLottieController::setWindow(OHNativeWindow *window)
{
    //TODO:lipei------------------【测试获取 UI 启动失败，闪退问题】-----------------------
    std::lock_guard lock_guard(locker);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【CJLottieController::setWindow】----------1--window == NULL: %{public}d", window == NULL);
    if (window) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【CJLottieController::setWindow】----------2--window: %{public}p", &window);
        this->window = window;
    }
    
}

uint64_t CJLottieController::getSurfaceWidth()
{
    std::lock_guard lock_guard(locker);
    return surfaceWidth; 
}
void CJLottieController::setSurfaceWidth(uint64_t surfaceWidth)
{ 
    std::lock_guard lock_guard(locker);
    this->surfaceWidth = surfaceWidth;
}

uint64_t CJLottieController::getSurfaceHeight()
{ 
    std::lock_guard lock_guard(locker);
    return surfaceHeight; 
}
void CJLottieController::setSurfaceHeight(uint64_t surfaceHeight)
{ 
    std::lock_guard lock_guard(locker);
    this->surfaceHeight = surfaceHeight;
}

std::string CJLottieController::getLottieId()
{
    std::lock_guard lock_guard(locker);
    return lottieId_;
}
void CJLottieController::setLottieId(std::string lottieId)
{
    std::lock_guard lock_guard(locker);
    this->lottieId_ = lottieId;
}

std::shared_ptr<LottieXcomponentNode> CJLottieController::getLottieXcomponentNode() {
    std::lock_guard lock_guard(locker);
    return this->lottieXcomponentNode_;
}

void CJLottieController::setLottieXcomponentNode(std::shared_ptr<LottieXcomponentNode> lottieXcomponentNode)
{
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【CJLottieController::setLottieXcomponentNode】----------1--lottieXcomponentNode: %{public}p", &lottieXcomponentNode);
    std::lock_guard lock_guard(locker);
    this->lottieXcomponentNode_ = lottieXcomponentNode;
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【CJLottieController::setLottieXcomponentNode】----------2--LottieXcomponentNode 指针赋值。");
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【CJLottieController::setLottieXcomponentNode】----------3--lottieXcomponentNode.use_count: %{public}lld", lottieXcomponentNode.use_count());
    
    //TODO:lipei------------------【测试获取 std::shared_ptr<LottieXcomponentNode> 指针问题】-----------------------
    ////std::shared_ptr<LottieXcomponentNode> lxnode = getLottieXcomponentNode();
    //std::shared_ptr<LottieXcomponentNode> lxnode = this->lottieXcomponentNode_;
    ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【CJLottieController::setLottieXcomponentNode】----------4--lxnode == null: %{public}d", lxnode == NULL);
    ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【CJLottieController::setLottieXcomponentNode】----------5--lxnode: %{public}p", lxnode.get());
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

int64_t cj_LottieController_init()
{
    //LottieControllerBridge *controller = new LottieControllerBridge();
    auto controller = std::make_shared<LottieControllerBridge>();
    CJLottieController *handler = new CJLottieController(controller);
    return (int64_t)handler;
    //auto cjViewController = std::make_shared<CJLottieController>(controller);
    //return reinterpret_cast<int64_t>(cjViewController.get());
}

void cj_LottieController_onSurfaceCreated(const int64_t jView, const int64_t surfaceId, uint64_t surfaceWidth, uint64_t surfaceHeight)
{
    if (jView == 0) {
        return;
    }
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【cj_LottieController_onSurfaceCreated】---------- onSurfaceCreated 触发");
    intptr_t viewPtrInt = static_cast<intptr_t>(jView);
    CJLottieController *view = reinterpret_cast<CJLottieController *>(viewPtrInt);
    if (!view) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieController_onSurfaceCreated", "CJLottieController is null.");
        return;
    }

    OHNativeWindow *window = nullptr;
    OH_NativeWindow_CreateNativeWindowFromSurfaceId(surfaceId, &window);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【cj_LottieController_onSurfaceCreated】---------- 存储 OHNativeWindow 指针。");
    view->setWindow(window);
    //this->window = window;
    
    //view->setSurfaceWidth(surfaceWidth);
    //view->setSurfaceHeight(surfaceHeight);

    //TODO:lipei------【测试中】放开 view->get()编译崩溃 ----------------------------------------------------------------
    std::shared_ptr<LottieXcomponentNode> lxnode = view->getLottieXcomponentNode();
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "--------------lxnode == null: %{public}d", lxnode == NULL);
    if (!lxnode) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieController_onSurfaceCreated", "get the LottieXcomponentNode is None.");
        return;
    }
    lxnode->OnSurfaceCreated(window, view->getLottieId(), surfaceWidth, surfaceHeight);
}

void cj_LottieController_onSurfaceChanged(const int64_t jView, const int64_t surfaceId, uint64_t surfaceWidth, uint64_t surfaceHeight)
{
    if (jView == 0) {
        return;
    }
    intptr_t viewPtrInt = static_cast<intptr_t>(jView);
    CJLottieController *view = reinterpret_cast<CJLottieController *>(viewPtrInt);
    if (!view) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieController_onSurfaceChanged", "view is null");
        return;
    }
    
    view->setSurfaceWidth(surfaceWidth);
    view->setSurfaceHeight(surfaceHeight);

    OHNativeWindow *window = view->getWindow();
    view->getLottieXcomponentNode()->OnSurfaceChanged(window, view->getLottieId(), surfaceWidth, surfaceHeight);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

CJBytesArray cj_LottieController_getId(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[getId]-the pointer of LottieController is null.");
        return {size: 0};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[getId]-LottieControllerBridge is null.");
        return {size: 0};
    }
    return instance->GetId();
}

bool cj_LottieController_setId(const int64_t ptr, char *lottieId)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setId]-the pointer of LottieController is null.");
        return false;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "[cj_LottieController_setId]--------------lottieId: %{public}s", lottieId);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setId]-LottieControllerBridge is null.");
        return false;
    }
    //TODO:lipei------------------【测试获取 std::shared_ptr<LottieXcomponentNode> 指针问题】-----------------------
    instance->SetId(lottieId);
    controller->setLottieId(lottieId);
    return true;
}

CJBoolStruct cj_LottieController_isLoaded(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[isLoaded]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[isLoaded]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetLoaded();
}

CJFloat64Struct cj_LottieController_get_currentFrame(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_currentFrame]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_currentFrame]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetCurrentFrame();
}

void cj_LottieController_set_currentFrame(const int64_t ptr, double currentFrame)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[set_currentFrame]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[set_currentFrame]-LottieControllerBridge is null.");
        return;
    }
    return instance->SetCurrentFrame(currentFrame);
}

CJFloat64Struct cj_LottieController_get_currentRawFrame(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_currentRawFrame]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_currentRawFrame]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetCurrentRawFrame();
}


void cj_LottieController_set_currentRawFrame(const int64_t ptr, double currentRawFrame)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[set_currentRawFrame]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[set_currentRawFrame]-LottieControllerBridge is null.");
        return;
    }
    return instance->SetCurrentRawFrame(currentRawFrame);
}

CJFloat64Struct cj_LottieController_get_firstFrame(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_firstFrame]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_firstFrame]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetFirstFrame();
}

CJFloat64Struct cj_LottieController_get_totalFrames(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_totalFrames]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_totalFrames]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetTotalFrames();
}

CJFloat64Struct cj_LottieController_get_frameRate(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_frameRate]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_frameRate]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetDataFrameRate();
}

CJFloat64Struct cj_LottieController_get_frameMult(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_frameMult]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_frameMult]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetFrameMult();
}

CJFloat64Struct cj_LottieController_get_playSpeed(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_playSpeed]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_playSpeed]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetPlaySpeed();
}

CJInt64Struct cj_LottieController_get_playDirection(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_playDirection]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_playDirection]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetPlayDirection();
}

CJInt64Struct cj_LottieController_get_playCount(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_playCount]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_playCount]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetPlayCount();
}

CJBoolStruct cj_LottieController_get_isPaused(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_isPaused]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_isPaused]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetIsPaused();
}

CJBoolStruct cj_LottieController_get_autoplay(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_autoplay]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_autoplay]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetAutoPlay();
}

CJInt64Struct cj_LottieController_get_loop(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_loop]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_loop]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetLoop();
}

CJFloat64Struct cj_LottieController_get_timeCompleted(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_loop]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_loop]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetTimeCompleted();
}

CJInt64Struct cj_LottieController_get_segmentPos(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_segmentPos]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_segmentPos]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetSegmentPos();
}

CJBoolStruct cj_LottieController_get_isSubframeEnabled(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_isSubframeEnabled]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_isSubframeEnabled]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetIsSubframeEnabled();
}

CJAnimationSegmentArray cj_LottieController_get_segments(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_isSubframeEnabled]-the pointer of LottieController is null.");
        return {size: 0};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[get_isSubframeEnabled]-LottieControllerBridge is null.");
        return {size: 0};
    }
    return instance->GetSegments();
}






//---------------------------------------------------------------------------------------------------


void cj_LottieController_play(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[play]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[play]-LottieControllerBridge is null.");
        return;
    }
    instance->Play();
}

void cj_LottieController_stop(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[stop]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[stop]-LottieControllerBridge is null.");
        return;
    }
    instance->Stop();
}

void cj_LottieController_togglePause(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[togglePause]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[togglePause]-LottieControllerBridge is null.");
        return;
    }
    instance->TogglePause();
}

void cj_LottieController_destroy(const int64_t ptr)
{
    if (ptr <= 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[destroy]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[destroy]-LottieControllerBridge is null.");
        return;
    }
    instance->Destroy();
}

void cj_LottieController_pause(const int64_t ptr)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[pause]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[pause]-LottieControllerBridge is null.");
        return;
    }
    instance->Pause();
}

void cj_LottieController_goToAndStop(const int64_t ptr, double value, bool isFrame)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[goToAndStop]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[goToAndStop]-LottieControllerBridge is null.");
        return;
    }
    instance->GoToAndStop(value, isFrame);
}

void cj_LottieController_goToAndPlay(const int64_t ptr, double value, bool isFrame)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[goToAndPlay]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[goToAndPlay]-LottieControllerBridge is null.");
        return;
    }
    instance->GoToAndPlay(value, isFrame);
}

void cj_LottieController_setSegment(const int64_t ptr, double start, double end)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setSegment]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setSegment]-LottieControllerBridge is null.");
        return;
    }
    instance->SetSegment(start, end);
}

void cj_LottieController_playSegments(const int64_t ptr, AnimationSegment segments[], const int64_t len, bool forceFlag, bool hasForceFlag)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[playSegments]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[playSegments]-LottieControllerBridge is null.");
        return;
    }
    instance->PlaySegments(segments, len, forceFlag, hasForceFlag);
}

void cj_LottieController_resetSegments(const int64_t ptr, bool forceFlag)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[resetSegments]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[resetSegments]-LottieControllerBridge is null.");
        return;
    }
    instance->ResetSegments(forceFlag);
}

void cj_LottieController_setSpeed(const int64_t ptr, double speed)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setSpeed]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setSpeed]-LottieControllerBridge is null.");
        return;
    }
    instance->SetSpeed(speed);
}

void cj_LottieController_setDirection(const int64_t ptr, const int64_t direction)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setDirection]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setDirection]-LottieControllerBridge is null.");
        return;
    }
    instance->SetDirection(direction);
}

bool cj_LottieController_setSubframe(const int64_t ptr, bool useSubFrames)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setSubframe]-the pointer of LottieController is null.");
        return false;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setSubframe]-LottieControllerBridge is null.");
        return false;
    }
    return instance->SetSubframe(useSubFrames);
}

CJFloat64Struct cj_LottieController_getDuration(const int64_t ptr, bool inFrames)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[getDuration]-the pointer of LottieController is null.");
        return {callResult: false};
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[getDuration]-LottieControllerBridge is null.");
        return {callResult: false};
    }
    return instance->GetDuration(inFrames);
}

void cj_LottieController_triggerEvent(const int64_t ptr, char *name)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[triggerEvent]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[triggerEvent]-LottieControllerBridge is null.");
        return;
    }
    return instance->TriggerEvent(name);
}

void cj_LottieController_addEventListener(const int64_t ptr, char *name, CallbackFunc callback)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[addEventListener]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[addEventListener]-LottieControllerBridge is null.");
        return;
    }
    return instance->AddEventListener(name, callback);
}

void cj_LottieController_removeEventListener(const int64_t ptr, char *name, CallbackFunc callback, bool hasCallbackFunc)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[removeEventListener]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[removeEventListener]-LottieControllerBridge is null.");
        return;
    }
    return instance->RemoveEventListener(name, callback, hasCallbackFunc);
}

void cj_LottieController_changeColor(const int64_t ptr, char *layer, double *startColor, const int64_t startColorLen, 
                                     double *endColor, const int64_t endColorLen)
{
    if (ptr <= 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[changeColor]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[changeColor]-LottieControllerBridge is null.");
        return;
    }
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[changeColor测试]------------------[cj_LottieController_changeColor]-0");
    return instance->ChangeColor(layer, startColor, startColorLen, endColor, endColorLen);
}

void cj_LottieController_setContentMode(const int64_t ptr, char *contentMode)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setContentMode]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setContentMode]-LottieControllerBridge is null.");
        return;
    }
    return instance->SetContentMode(contentMode);
}

void cj_LottieController_setFrameRate(const int64_t ptr, double frameRate)
{
    if (ptr <= 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setFrameRate]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[setFrameRate]-LottieControllerBridge is null.");
        return;
    }
    return instance->SetFrameRate(frameRate);
}

void cj_LottieController_reload(const int64_t ptr, LottieDataConfig config)
{
    if (ptr <= 0) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[reload]-the pointer of LottieController is null.");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance == NULL) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "[reload]-LottieControllerBridge is null.");
        return;
    }
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "--cj_LottieController_reload--00----");

    return instance->ReLoad(config);
}




//----------------------------------------------------------------------------------------------------------------------


void cj_LottieController_release(const int64_t ptr)
{
    if (ptr == 0) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "Pointer is null");
        return;
    }
    intptr_t ptrInt = static_cast<intptr_t>(ptr);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    std::shared_ptr<LottieControllerBridge> instance = controller->getLottieControllerBridge();
    if (instance) {
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "[CJLottieController]", "Invalid instance");
        return;
    }
    delete instance.get();
}


};

};
