/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "animator.h"


extern "C" {
namespace LottieTurbo {

Animator::~Animator()
{
    status_ = AnimatorStatus::END;
    Stop();
    //只能用 ohos.animator（动画） 中的 func cancel(): Unit 函数替换。
    if (handleAnimator) {
        handleAnimator(cancelAnimator, g_jAnimator);
    }
}

//void Animator::SetDuration(int32_t duration)
//{
//    if (status_ == AnimatorStatus::END) {
//        return;
//    }
//    //int32_t eCode = 0;
//    ////TODO:lipei----------------------只能用 ohos.animator（动画） 中的 func reset(options: AnimatorOptions): Unit 函数替换。
//    //eCode = OH_ArkUI_AnimatorOption_SetDuration(animatorOption_, duration);
//    //if (eCode != 0) {
//    //    DRAWING_LOGE("Animator::SetDuration: 启动animator动画%{public}d", eCode);
//    //}
//    //return eCode;
//}

void Animator::Play()
{
    if (status_ == AnimatorStatus::PLAY) {
        return;
    }
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【Animator::Play】----------4- handleAnimator == NULL : %{public}d", handleAnimator == NULL);
    //只能用 ohos.animator（动画） 中的 func play(): Unit 函数替换。
    if (handleAnimator) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【Animator::Play】----------4-1");
        handleAnimator(playAnimator, g_jAnimator);
    }
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【Animator::Play】----------4-2");
    status_ = AnimatorStatus::PLAY;
}

void Animator::Pause()
{
    if (status_ == AnimatorStatus::PAUSE) {
        return;
    }
    //只能用 ohos.animator（动画） 中的 func pause(): Unit 函数替换。
    if (handleAnimator) {
        handleAnimator(pauseAnimator, g_jAnimator);
    }
    status_ = AnimatorStatus::PAUSE;
}

void Animator::Stop()
{
    if (status_ == AnimatorStatus::STOP) {
        return;
    }
    //if (animatorHandle_ == nullptr) {
    //    //DRAWING_LOGE("Animator::Stop failed, animatorHandle_ is nullptr");
    //    return;
    //}
    //只能用 ohos.animator（动画） 中的 func finish(): Unit 函数替换。
    if (handleAnimator) {
        handleAnimator(stopAnimator, g_jAnimator);
    }
    status_ = AnimatorStatus::STOP;
}

const Animator::AnimatorStatus Animator::GetStatus() const
{
    return status_;
}

void Animator::SetFrameRateRange(ArkUI_ExpectedFrameRateRange frameRateRange)
{
    if (status_ == AnimatorStatus::END) {
        return;
    }
    frameRateRange_ = frameRateRange;
}

void Animator::UpdateFrameRateRange()
{
    if (status_ == AnimatorStatus::END) {
        return;
    }
    //if (animatorHandle_ == nullptr) {
    //    return;
    //}
    //只能用 ohos.animator（动画）中的 func setExpectedFrameRateRange(framerateRange: ExpectedFrameRateRange): Unit 函数替换。
    if (setAnimatorFrameRate) {
        setAnimatorFrameRate(g_jAnimator, frameRateRange_.expected);
    }
    //TODO:lipei----------------------只能用 ohos.animator（动画） 中的 func reset(options: AnimatorOptions): Unit 函数替换。
}

//TODO:lipei----------------------只能用 ohos.animator（动画） 中的 AnimatorResult(options: AnimatorOptions) 动画类初始化替代。
void Animator::Init()
{
    if (status_ != AnimatorStatus::IDLE) {
        return;
    }
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【Animator::Init】----------创建 ohos.animator 实例--1-0");
    ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【Animator::Init】----------创建 ohos.animator 实例--1-0-handleAnimator == NULL : %{public}d", handleAnimator == NULL);
    //if (handleAnimator) {
    //    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【Animator::Init】----------创建 ohos.animator 实例--1-1");
    //    handleAnimator(createAnimator, g_jAnimator);
    //}
    status_ = AnimatorStatus::INIT;
}
//void Animator::Init(ArkUI_ContextHandle context)
//{
//    if (status_ != AnimatorStatus::IDLE) {
//        return;
//    }
//    int32_t eCode = 0;
//    // 创建ArkUI_AnimatorHandle
//    animateAPi_1_ = reinterpret_cast<ArkUI_NativeAnimateAPI_1 *>(
//                    OH_ArkUI_QueryModuleInterfaceByName(ARKUI_NATIVE_ANIMATE, "ArkUI_NativeAnimateAPI_1"));
//    if (animateAPi_1_ == nullptr) {
//        DRAWING_LOGE("Animator::Init: AnimateAPi_1创建错误");
//        return;
//    }
//
//    // 创建动画控制选项
//    animatorOption_ = OH_ArkUI_AnimatorOption_Create(0);
//
//    // 设置animator动画持续时间
//    eCode = OH_ArkUI_AnimatorOption_SetDuration(animatorOption_, duration_);
//    if (eCode != 0) {
//        DRAWING_LOGE("\n\n\nAnimator::Init: 设置animator动画持续时间 错误代码%{public}d", eCode);
//    }
//
//    // 设置animator动画延迟时间
//    eCode = OH_ArkUI_AnimatorOption_SetDelay(animatorOption_, delay_);
//    if (eCode != 0) {
//        DRAWING_LOGE("\n\n\nAnimator::Init: 设置animator动画期望延迟时间 错误代码%{public}d", eCode);
//    }
//
//    // 设置animator动画播放次数
//    eCode = OH_ArkUI_AnimatorOption_SetIterations(animatorOption_, iterations_);
//    if (eCode != 0) {
//        DRAWING_LOGE("\n\n\nAnimator::Init: 设置animator动画播放次数 错误代码%{public}d", eCode);
//    }
//
//    // 设置animator动画遮罩
//    eCode = OH_ArkUI_AnimatorOption_SetFill(animatorOption_, fill_);
//    if (eCode != 0) {
//        DRAWING_LOGE("\n\n\nAnimator::Init: 设置animator动画动画遮罩 错误代码%{public}d", eCode);
//    }
//
//    // 设置animator帧率范围
//    this->UpdateFrameRateRange();
//
//    // 设置animator 帧回调
//    eCode = OH_ArkUI_AnimatorOption_RegisterOnFrameCallback(animatorOption_, this, OnCallBack);
//    if (eCode != 0) {
//        DRAWING_LOGE("\n\n\nAnimator::Init: 设置animator动画帧回调 错误代码%{public}d", eCode);
//    }
//
//    // 设置animator播放方向
//    eCode = OH_ArkUI_AnimatorOption_SetDirection(animatorOption_, direction_);
//    if (eCode != 0) {
//        DRAWING_LOGE("\n\n\nAnimator::Init: 设置animator动画播放方向 错误代码%{public}d", eCode);
//    }
//
//    // 设置animator ArkUI_ContextHandle
//    if (animateAPi_1_ != nullptr && animateAPi_1_->createAnimator != nullptr && context != nullptr) {
//        animatorHandle_ = animateAPi_1_->createAnimator(context, animatorOption_);
//    }
//    status_ = AnimatorStatus::INIT;
//}

// 设置animator 帧回调
//TODO:lipei----------------------只能用 ohos.animator（动画） 中的 prop onFrame: (Float64) -> Unit （接收到帧时回调）替代。
//void Animator::OnCallBack(ArkUI_AnimatorOnFrameEvent *event)
//{
//    void *res = OH_ArkUI_AnimatorOnFrameEvent_GetUserData(event);
//    Animator *data = reinterpret_cast<Animator *>(res);
//    if (FunctionCallBack != nullptr) {
//        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【Animator::OnCallBack】----------【aa】");
//        data->FunctionCallBack();
//    }
//}
void Animator::OnCallBack()
{
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【Animator::OnCallBack】----------【aa】-0");
    if (FunctionCallBack != nullptr) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【Animator::OnCallBack】----------【aa】");
        FunctionCallBack();
    }
}



}
}

