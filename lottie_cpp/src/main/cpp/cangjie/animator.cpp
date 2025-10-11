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

void Animator::Play()
{
    if (status_ == AnimatorStatus::PLAY) {
        return;
    }
    //只能用 ohos.animator（动画） 中的 func play(): Unit 函数替换。
    if (handleAnimator) {
        handleAnimator(playAnimator, g_jAnimator);
    }
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

    //只能用 ohos.animator（动画）中的 func setExpectedFrameRateRange(framerateRange: ExpectedFrameRateRange): Unit 函数替换。
    if (setAnimatorFrameRate) {
        setAnimatorFrameRate(g_jAnimator, frameRateRange_.expected);
    }
    //只能用 ohos.animator（动画） 中的 func reset(options: AnimatorOptions): Unit 函数替换。
}

//只能用 ohos.animator（动画） 中的 AnimatorResult(options: AnimatorOptions) 动画类初始化替代。
void Animator::Init()
{
    if (status_ != AnimatorStatus::IDLE) {
        return;
    }

    status_ = AnimatorStatus::INIT;
}

// 设置animator 帧回调
//只能用 ohos.animator（动画） 中的 prop onFrame: (Float64) -> Unit （接收到帧时回调）替代。
void Animator::OnCallBack()
{
    if (FunctionCallBack != nullptr) {
        FunctionCallBack();
    }
}

}
}
