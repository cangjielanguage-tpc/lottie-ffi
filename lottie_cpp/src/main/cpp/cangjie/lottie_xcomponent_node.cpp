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
}

void LottieXcomponentNode::CreateXcomponentNode(std::string xcomponentId, float radius)
{
    if (xcomponentId.length() <= 0) {
        DRAWING_LOGE("LottieXcomponentNode::CreateXcomponentNode lottieId(%{public}s): Error xcomponentId NULL",
                     !lottieHandler_ ? "" : lottieHandler_->GetLottieId().c_str());
        return;
    }

    LottieManagerMap::getInstance().Add(xcomponentId);
    lottieRender_ = std::make_shared<LottieRender>();
}

void LottieXcomponentNode::SetAnimator()
{
    if (!lottieHandler_) {
        DRAWING_LOGE("LottieXcomponentNode::SetAnimator lottieId(%{public}s): Error Init params NULL",
                     !lottieHandler_ ? "" : lottieHandler_->GetLottieId().c_str());
        return;
    }
    lottieHandler_->SetAnimator();
}

void LottieXcomponentNode::SetHandler(std::shared_ptr<LottieHandler> lottieHandler)
{
    if (!lottieHandler) {
        DRAWING_LOGE("LottieXcomponentNode::SetHandle lottieId(%{public}s): Error Init params NULL",
                     !lottieHandler ? "" : lottieHandler->GetLottieId().c_str());
        return;
    }
    lottieHandler_ = lottieHandler;
}

void LottieXcomponentNode::Init()
{
    status_ = 0;
    int32_t width = 0;
    int32_t height = 0;
    if (!lottieHandler_) {
        DRAWING_LOGE("LottieXcomponentNode::Init lottieId(%{public}s): Error Init params NULL",
                     !lottieHandler_ ? "" : lottieHandler_->GetLottieId().c_str());
        return;
    }

    lottieHandler_->SetRenderCallBack([this]() { this->RenderCurrentFrame(); });
    lottieHandler_->SetAnimatorCallBack([this]() { this->RenderAndAdvanceFrame(); });
    lottieHandler_->SetContentModeUpdateCallback([this]() { this->OnUpdateContentMode(); });
    LottieManagerMap::getInstance().SetHandleById(lottieHandler_->GetLottieId(), lottieHandler_);
    LottieManagerMap::getInstance().GetRectById(lottieHandler_->GetLottieId(), width, height);

    if (width != 0 && height != 0) {
        lottieHandler_->SetLottieSize(width, height);
        if (lottieHandler_->updateContentMode) {
            lottieHandler_->updateContentMode();
        }
        if (lottieHandler_->GetLottieConfig()->GetAutoplay()) {
            lottieHandler_->Play();
        } else if (lottieHandler_->renderAndAdvanceFrame) {
            lottieHandler_->renderAndAdvanceFrame();
        }
    }
}

void LottieXcomponentNode::RenderCurrentFrame()
{
    if (!lottieHandler_ || !lottieHandler_->dataManager_ || !lottieHandler_->lottieListener_) {
        DRAWING_LOGE("LottieXcomponentNode::RenderCurrentFrame lottieId(%{public}s): Error OnDraw params NULL",
                     !lottieHandler_ ? "" : lottieHandler_->GetLottieId().c_str());
        return;
    }
    float absoluteFrameToPlay = static_cast<float>(lottieHandler_->GetAbsoluteFrameToPlay());
    if (!lottieHandler_->GetIsSubframeEnabled()) {
        absoluteFrameToPlay = std::round(absoluteFrameToPlay);
    }
    uint32_t lottieFileTotalFrame = static_cast<uint32_t>(lottieHandler_->GetLottieFileTotalFrames());
    DRAWING_LOGD("OnDraw, absoluteFrameToPlay: %{public}f, lottieFileTotalFrame: %{public}d", absoluteFrameToPlay, lottieFileTotalFrame);
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

        auto lotLayerNode = handler->dataManager_->player->renderTree(
                                ab, position.width, position.height, position.keepAspectRatio);
        render->RenderTree(handler->GetLottieId(), lotLayerNode, (LottieTurbo::CalculatePosition *)&position);

        auto func = [](LottieListener *listener, LottieEventType type) {
            if (listener) {
                listener->TriggerEvent(type);
            }
        };
        auto trigger = new LottieHandler::TriggerData;
        trigger->func = std::bind(func, handler->lottieListener_.get(), LottieEventType::ON_DRAW_FRAME);
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
    if (lottieHandler_->lottieListener_) {
        auto func = [](LottieListener *listener, LottieEventType type) {
            if (listener) {
                listener->TriggerEvent(type);
            }
        };
        auto trigger = new LottieHandler::TriggerData;
    }

    if (lottieHandler_ && lottieHandler_->animator_ &&
        (lottieHandler_->animator_->GetStatus() == LottieTurbo::Animator::AnimatorStatus::PLAY ||
         lottieHandler_->animator_->GetStatus() == LottieTurbo::Animator::AnimatorStatus::INIT)) {
        this->RenderCurrentFrame();
        lottieHandler_->AdvanceFrame();
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
    if (!idStr) {
        DRAWING_LOGE("LottieXcomponentNode::RenderOnSurfaceChanged: Error GetXComponentId Failed");
        return;
    }

    uint64_t width = surfaceWidth;
    uint64_t height = surfaceHeight;
    if (width <= 0 || height <= 0) {
        DRAWING_LOGE("LottieXcomponentNode::RenderOnSurfaceChanged: Error GetXComponentSize Failed");
        return;
    }
    
    DRAWING_LOGD("RenderOnSurfaceChanged ret=%{public}d width=%{public}lu,"
                 "height=%{public}lu id=%{public}s",
                 ret, width, height, idStr);
    if (isOnSurfaceCreated) {
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
        } else if (!isOnSurfaceCreated && lottieHandler->renderCurrentFrame && lottieHandler->GetIsPaused()) {
            lottieHandler->renderCurrentFrame();
        } else if (lottieHandler->renderAndAdvanceFrame) {
            lottieHandler->renderAndAdvanceFrame();
        }
    } else {
        DRAWING_LOGW("LottieXcomponentNode::RenderOnSurfaceChanged: Warning LottieHandler not find");
    }
}

void LottieXcomponentNode::OnSurfaceCreated(void *window, std::string lottieId, uint64_t surfaceWidth, uint64_t surfaceHeight)
{
    DRAWING_LOGD("OnSurfaceCreated");
    RenderOnSurfaceChanged(window, lottieId, surfaceWidth, surfaceHeight, true);
}

void LottieXcomponentNode::OnSurfaceChanged(void *window, std::string lottieId, uint64_t surfaceWidth, uint64_t surfaceHeight)
{
    DRAWING_LOGD("OnSurfaceChanged");
    RenderOnSurfaceChanged(window, lottieId, surfaceWidth, surfaceHeight, false);
}

void LottieXcomponentNode::destroy(uint64_t surfaceWidth, uint64_t surfaceHeight)
{
    lottieRender_->destroy(lottieHandler_->GetLottieId(), (LottieTurbo::CalculatePosition *)&position_);
}


}
