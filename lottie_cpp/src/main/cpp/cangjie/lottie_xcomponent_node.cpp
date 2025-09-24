/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "lottie_xcomponent_node.h"

#include "node/lottie_manager.h"
#include "task_pool/task_pool.h"
#include <arkui/native_interface.h>
#include "lottie_listener.h"
#include <cstdint>


namespace LottieTurbo {
extern "C" { // cangjie [LottieXcomponentNode] ffi



}


LottieXcomponentNode::LottieXcomponentNode(std::string xcomponentId, float radius)
{
    this->CreateXcomponentNode(xcomponentId, radius);
}

LottieXcomponentNode::~LottieXcomponentNode()
{
    status_ = 1;
    //arkUINativeNodeApi_->disposeNode(handle_);
}

void LottieXcomponentNode::CreateXcomponentNode(std::string xcomponentId, float radius)
{
    //OH_ArkUI_GetModuleInterface(ARKUI_NATIVE_NODE, ArkUI_NativeNodeAPI_1, arkUINativeNodeApi_);
    //if (!arkUINativeNodeApi_) {
    //    //DRAWING_LOGE("LottieXcomponentNode::CreateXcomponentNode lottieId(%{public}s): Error arkUINativeNodeApi_ NULL",
    //    //             !lottieHandler_ ? "" : lottieHandler_->GetLottieId().c_str());
    //    return;
    //}
    //handle_ = arkUINativeNodeApi_->createNode(ARKUI_NODE_XCOMPONENT);
    //if (!handle_) {
    //    DRAWING_LOGE("LottieXcomponentNode::CreateXcomponentNode lottieId(%{public}s): Error handle_ NULL",
    //                 !lottieHandler_ ? "" : lottieHandler_->GetLottieId().c_str());
    //    return;
    //}
    if (xcomponentId.length() <= 0) {
        //DRAWING_LOGE("LottieXcomponentNode::CreateXcomponentNode lottieId(%{public}s): Error xcomponentId NULL",
        //             !lottieHandler_ ? "" : lottieHandler_->GetLottieId().c_str());
        return;
    }
    //ArkUI_AttributeItem item = {nullptr, NUM_1, xcomponentId.c_str()};
    //arkUINativeNodeApi_->setAttribute(handle_, NODE_XCOMPONENT_ID, &item);
    //ArkUI_NumberValue backgroundColorValue[] = {{.u32 = 0x00000000}};
    //ArkUI_AttributeItem backgroundColorItem = {backgroundColorValue,
    //                                           sizeof(backgroundColorValue) / sizeof(ArkUI_NumberValue)};
    //arkUINativeNodeApi_->setAttribute(handle_, NODE_BACKGROUND_COLOR, &backgroundColorItem);
    //if (radius > 0) {
    //    DRAWING_LOGE("lal lottieId(%{public}f)", radius);
    //    ArkUI_NumberValue radiusValue[] = {{.f32 = radius}};
    //    ArkUI_AttributeItem radiusItem = {radiusValue, sizeof(radiusValue) / sizeof(ArkUI_NumberValue)};
    //    arkUINativeNodeApi_->setAttribute(handle_, NODE_BORDER_RADIUS, &radiusItem);
    //}
    //OH_NativeXComponent_Callback *callback = new OH_NativeXComponent_Callback;
    //callback->OnSurfaceCreated = OnSurfaceCreated;
    //callback->OnSurfaceChanged = OnSurfaceChanged;
    //callback->DispatchTouchEvent = OnDispatchTouchEvent;
    //callback->OnSurfaceDestroyed = OnSurfaceDestroyed;
    //PushCallback(xcomponentId, callback);
    //nativeXComponent_ = OH_NativeXComponent_GetNativeXComponent(handle_);
    //if (!nativeXComponent_) {
    //    DRAWING_LOGE("LottieXcomponentNode::CreateXcomponentNode lottieId(%{public}s): Error GetNativeXComponent "
    //                 "nativeXComponent_ NULL",
    //                 !lottieHandler_ ? "" : lottieHandler_->GetLottieId().c_str());
    //    return;
    //}
    //OH_NativeXComponent_RegisterCallback(nativeXComponent_, callback);
    LottieManagerMap::getInstance().Add(xcomponentId);
    lottieRender_ = std::make_shared<LottieRender>();
}

//TODO:lipei--------------暂未实现-------------------------
void LottieXcomponentNode::SetAnimator()
{
    //if (!handle_ || !lottieHandler_) {
    if (!lottieHandler_) {
        DRAWING_LOGE("LottieXcomponentNode::SetAnimator lottieId(%{public}s): Error Init params NULL",
                     !lottieHandler_ ? "" : lottieHandler_->GetLottieId().c_str());
        return;
    }
    //ArkUI_ContextHandle contextHandle = OH_ArkUI_GetContextByNode(handle_); //获取指向节点所在页面的UIContext指针。
    //if (!contextHandle) {
    //    DRAWING_LOGE("LottieXcomponentNode::SetAnimator lottieId(%{public}s): Error contextHandle NULL",
    //                 lottieHandler_->GetLottieId().c_str());
    //    return;
    //}
    //lottieHandler_->SetAnimator(contextHandle);
    lottieHandler_->SetAnimator();
}

void LottieXcomponentNode::SetHandler(std::shared_ptr<LottieHandler> lottieHandler)
{
    //if (!handle_ || !lottieHandler) {
    if (!lottieHandler) {
        //DRAWING_LOGE("LottieXcomponentNode::SetHandle lottieId(%{public}s): Error Init params NULL",
        //             !lottieHandler ? "" : lottieHandler->GetLottieId().c_str());
        return;
    }
    lottieHandler_ = lottieHandler;
}

void LottieXcomponentNode::Init()
{
    status_ = 0;
    int32_t width = 0;
    int32_t height = 0;
    //if (!handle_ || !lottieHandler_) {
    if (!lottieHandler_) {
        //DRAWING_LOGE("LottieXcomponentNode::Init lottieId(%{public}s): Error Init params NULL",
        //             !lottieHandler_ ? "" : lottieHandler_->GetLottieId().c_str());
        return;
    }
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::Init】----------2");
    lottieHandler_->SetRenderCallBack([this]() { this->RenderCurrentFrame(); });
    lottieHandler_->SetAnimatorCallBack([this]() { this->RenderAndAdvanceFrame(); });
    lottieHandler_->SetContentModeUpdateCallback([this]() { this->OnUpdateContentMode(); });
    LottieManagerMap::getInstance().SetHandleById(lottieHandler_->GetLottieId(), lottieHandler_);
    LottieManagerMap::getInstance().GetRectById(lottieHandler_->GetLottieId(), width, height);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::Init】----------2-0");
    if (width != 0 && height != 0) {
        lottieHandler_->SetLottieSize(width, height);
        if (lottieHandler_->updateContentMode) {
            lottieHandler_->updateContentMode();
        }
        if (lottieHandler_->GetLottieConfig()->GetAutoplay()) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::Init】----------2-1");
            lottieHandler_->Play();
        } else if (lottieHandler_->renderAndAdvanceFrame) {
            OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::Init】----------2-2");
            lottieHandler_->renderAndAdvanceFrame();
        }
    }
}

void LottieXcomponentNode::RenderCurrentFrame()
{
    if (!lottieHandler_ || !lottieHandler_->dataManager_ || !lottieHandler_->lottieListener_) {
        //DRAWING_LOGE("LottieXcomponentNode::RenderCurrentFrame lottieId(%{public}s): Error OnDraw params NULL",
        //             !lottieHandler_ ? "" : lottieHandler_->GetLottieId().c_str());
        return;
    }
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderCurrentFrame】----------【cc】");
    float absoluteFrameToPlay = static_cast<float>(lottieHandler_->GetAbsoluteFrameToPlay());
    if (!lottieHandler_->GetIsSubframeEnabled()) {
        absoluteFrameToPlay = std::round(absoluteFrameToPlay);
    }
    uint32_t lottieFileTotalFrame = static_cast<uint32_t>(lottieHandler_->GetLottieFileTotalFrames());
    //DRAWING_LOGD("OnDraw, absoluteFrameToPlay: %{public}f, lottieFileTotalFrame: %{public}d", absoluteFrameToPlay, lottieFileTotalFrame);
    int64_t current = lottieHandler_->currentIndex_++;
    // 绝对帧大于动画文件总帧数时不绘制
    if (absoluteFrameToPlay < lottieFileTotalFrame) {
        ScheduleRenderTask(absoluteFrameToPlay, current);
    }
}

void LottieXcomponentNode::ScheduleRenderTask(float absoluteFrameToPlay, int64_t current)
{
    auto func = [](std::shared_ptr<LottieHandler> handler, CalculatePosition position,
                   std::shared_ptr<LottieRender> render, float ab, int64_t current) {
        /*
         * 这里添加丢帧逻辑，规则如下
         * 1. 当前需要渲染的帧和最新的一帧相差超过10帧，则开启丢帧模式
         * 2. 为了保证动画的连续性，丢帧时保证连续丢帧不超过N帧
         * 3. 通过总帧数 / 20 计算出来, N最大为3帧
         */
        auto loss = handler->GetTotalFrames() / handler->skipFrameBase;
        if (loss > handler->skipFrameLimits) {
            loss = handler->skipFrameLimits;
        }
        if (abs(current - handler->currentIndex_) >= handler->lossFrameLimits && handler->skip_ < loss) {
            handler->skip_++;
            return;
        }
        handler->skip_ = 0;

        if (handler->isDestroy_) {
            return;
        }

        if (handler->GetPlayState() == LottieHandler::PlayState::STOP ||
            handler->GetPlayState() == LottieHandler::PlayState::PAUSE) {
            handler->SetPlayState(LottieHandler::PlayState::END);
        }
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::ScheduleRenderTask】----------【dd】");
        //HITRACE(
        //    "Lottie::Render", auto lotLayerNode = handler->dataManager_->player->renderTree(
        //                        ab, position.width, position.height, position.keepAspectRatio);
        //    ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::ScheduleRenderTask】----------【dd】-1");    
        //    render->RenderTree(handler->GetLottieId(), lotLayerNode, (LottieTurbo::CalculatePosition *)&position);
        //);
        //HITRACE(
            //"Lottie::Render", 
        auto lotLayerNode = handler->dataManager_->player->renderTree(
                                ab, position.width, position.height, position.keepAspectRatio);
            OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::ScheduleRenderTask】----------【dd】-1");    
            render->RenderTree(handler->GetLottieId(), lotLayerNode, (LottieTurbo::CalculatePosition *)&position);
        //);

        auto func = [](LottieListener *listener, LottieEventType type) {
            if (listener) {
                listener->TriggerEvent(type);
            }
        };
        auto trigger = new LottieHandler::TriggerData;
        trigger->func = std::bind(func, handler->lottieListener_.get(), LottieEventType::ON_DRAW_FRAME);
        //napi_call_threadsafe_function(handler->safeFunc_, trigger, napi_tsfn_blocking);
    };
    TaskPoolLocal::GetInstance().PushTask(func, lottieHandler_, position_, lottieRender_, absoluteFrameToPlay, current);
}

void LottieXcomponentNode::RenderAndAdvanceFrame()
{
    if (firstFrame_) {
        firstFrame_ = false;
    } else if (lottieHandler_ && lottieHandler_->dataManager_) {
        if (lottieHandler_->dataManager_->GetVisible() == false) {
            lottieHandler_->ResetLastTimeStamp();
            return;
        }
    }
    if (status_) {
        return;
    }
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderAndAdvanceFrame】----------【bb】");
    if (lottieHandler_->lottieListener_) {
        auto func = [](LottieListener *listener, LottieEventType type) {
            if (listener) {
                listener->TriggerEvent(type);
            }
        };
        auto trigger = new LottieHandler::TriggerData;
        //trigger->func = std::bind(func, lottieHandler_->lottieListener_.get(), LottieEventType::ON_ENTER_FRAME);
        //napi_call_threadsafe_function(lottieHandler_->safeFunc_, trigger, napi_tsfn_blocking);
    }
    
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderAndAdvanceFrame】----------【bb】-lottieHandler_ != NULL: %{public}d", lottieHandler_ != NULL);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderAndAdvanceFrame】----------【bb】-lottieHandler_->animator_ != NULL: %{public}d", lottieHandler_->animator_ != NULL);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderAndAdvanceFrame】----------【bb】-status_ == PLAY: %{public}d", lottieHandler_->animator_->GetStatus() == LottieTurbo::Animator::AnimatorStatus::PLAY);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderAndAdvanceFrame】----------【bb】-status_ == INIT: %{public}d", lottieHandler_->animator_->GetStatus() == LottieTurbo::Animator::AnimatorStatus::INIT);
    
    if (lottieHandler_ && lottieHandler_->animator_ &&
        (lottieHandler_->animator_->GetStatus() == LottieTurbo::Animator::AnimatorStatus::PLAY ||
         lottieHandler_->animator_->GetStatus() == LottieTurbo::Animator::AnimatorStatus::INIT)) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderAndAdvanceFrame】----------【bb】-1");
        this->RenderCurrentFrame();
        lottieHandler_->AdvanceFrame();
    } else {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderAndAdvanceFrame】----------【bb】-2");
    }
}

void LottieXcomponentNode::OnUpdateContentMode()
{
    size_t animWidth;
    size_t animHeight;
    std::string contentMode = lottieHandler_->GetLottieConfig()->GetContentMode();
    lottieHandler_->dataManager_->GetSize(animWidth, animHeight);
    int componentWidth = lottieHandler_->GetLottieSize().width;
    int componentHeight = lottieHandler_->GetLottieSize().height;
    float animAspectRatio = static_cast<float>(animWidth) / animHeight;
    float componentAspectRatio = static_cast<float>(componentWidth) / componentHeight;

    // 初始化位置
    position_.x = 0;
    position_.y = 0;
    position_.width = componentWidth;
    position_.height = componentHeight;

    // 根据contentMode设置缩放和对齐方式
    if (contentMode == "Contain") {
        position_.keepAspectRatio = true;
    } else if (contentMode == "Fill") {
        position_.keepAspectRatio = false;
    } else if (contentMode == "Cover") {
        bool useWidthScale = (animAspectRatio <= componentAspectRatio);
        float scale = useWidthScale ?
            static_cast<float>(componentWidth) / animWidth :
            static_cast<float>(componentHeight) / animHeight;

        if (useWidthScale) {
            position_.height = static_cast<int>(animHeight * scale);
            position_.y = -(position_.height - componentHeight + PIXEL_OFFSET) / CENTER_DIVISOR;
        } else {
            position_.width = static_cast<int>(animWidth * scale);
            position_.x = -(position_.width - componentWidth + PIXEL_OFFSET) / CENTER_DIVISOR;
        }
    } else if (contentMode == "Top" || contentMode == "Bottom") {
        bool useWidthScale = (animAspectRatio >= componentAspectRatio);
        float scale = useWidthScale ?
            static_cast<float>(componentWidth) / animWidth :
            static_cast<float>(componentHeight) / animHeight;

        if (useWidthScale) {
            position_.height = static_cast<int>(animHeight * scale);
            if (contentMode == "Bottom") {
                position_.y = componentHeight - position_.height;
            }
        } else {
            position_.width = static_cast<int>(animWidth * scale);
            if (contentMode == "Bottom") {
                position_.x = componentWidth - position_.width;
            }
        }
    } else {
        position_.keepAspectRatio = true;
    }
    lottieHandler_->dataManager_->InitRect(position_.width, position_.height);
}

void LottieXcomponentNode::RenderOnSurfaceChanged(void *window, std::string lottieId, uint64_t surfaceWidth, uint64_t surfaceHeight, bool isOnSurfaceCreated)
{

    const char* idStr = lottieId.c_str();
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderOnSurfaceChanged】----------[定位]--3--idStr = %{public}s", idStr);
    if (!idStr) {
        DRAWING_LOGE("LottieXcomponentNode::RenderOnSurfaceChanged: Error GetXComponentId Failed");
        return;
    }

    uint64_t width = surfaceWidth;
    uint64_t height = surfaceHeight;
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderOnSurfaceChanged】----------[定位]--3-1--width = %{public}d", width);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderOnSurfaceChanged】----------[定位]--3-2--iheight = %{public}d", height);
    if (width <= 0 || height <= 0) {
        DRAWING_LOGE("LottieXcomponentNode::RenderOnSurfaceChanged: Error GetXComponentSize Failed");
        return;
    }
    
    //DRAWING_LOGD("RenderOnSurfaceChanged ret=%{public}d width=%{public}lu,"
    //             "height=%{public}lu id=%{public}s",
    //             ret, width, height, idStr);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderOnSurfaceChanged】----------[定位]--3-3");
    if (isOnSurfaceCreated) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::RenderOnSurfaceChanged】----------[定位]--3-4");
        LottieManagerMap::getInstance().SetWindowById(idStr, window);
    }
    
    LottieManagerMap::getInstance().SetRectById(idStr, width, height);
    auto lottieHandler = LottieManagerMap::getInstance().GetHandleById(idStr);
    if (lottieHandler != nullptr) {
        lottieHandler->SetLottieSize(width, height);
        if (lottieHandler->updateContentMode) {
            lottieHandler->updateContentMode();
        }
        if (isOnSurfaceCreated && lottieHandler->GetLottieConfig()->GetAutoplay()) {
            lottieHandler->Play();
        } else if (lottieHandler->renderAndAdvanceFrame) {
            lottieHandler->renderAndAdvanceFrame();
        }
    } else {
        DRAWING_LOGW("LottieXcomponentNode::RenderOnSurfaceChanged: Warning LottieHandler not find");
    }
}

void LottieXcomponentNode::OnSurfaceCreated(void *window, std::string lottieId, uint64_t surfaceWidth, uint64_t surfaceHeight)
{
    //DRAWING_LOGD("OnSurfaceCreated");
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::OnSurfaceCreated】----------[定位]--1--LottieId:%{public}s", lottieId.c_str());
    RenderOnSurfaceChanged(window, lottieId, surfaceWidth, surfaceHeight, true);
}

void LottieXcomponentNode::OnSurfaceChanged(void *window, std::string lottieId, uint64_t surfaceWidth, uint64_t surfaceHeight)
{
    //DRAWING_LOGD("OnSurfaceChanged");
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieXcomponentNode::OnSurfaceChanged】----------[定位]--2");
    RenderOnSurfaceChanged(window, lottieId, surfaceWidth, surfaceHeight, false);
}

//TODO:lipei--------------暂未实现-------------------------
//void LottieXcomponentNode::OnDispatchTouchEvent(OH_NativeXComponent *component, void *window)
//{
//    DRAWING_LOGD("OnDispatchTouchEvent");
//}

//TODO:lipei--------------暂未实现-------------------------
//void LottieXcomponentNode::OnSurfaceDestroyed(OH_NativeXComponent *component, void *window)

}
