/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "cj_animator.h"


extern "C" {
namespace LottieTurbo {

int64_t cj_LottieAnimator_createLottieAnimator(){
    std::shared_ptr<Animator> animator = std::make_shared<Animator>();
    CJAnimator *handler = new CJAnimator(animator);
    return (int64_t)handler;
}

/**
 * handle the animator.
 * state - 0: create - create an Animator.
 * state - 1: cancel - cancel the Animator.
 * state - 2: play   - Starts the animation of an animator.
 * state - 3: pause  - Pauses the animation of an animator.
 * state - 4: stop   - Ends the animation of an animator.
 */
void cj_LottieAnimator_handleAnimator(const int64_t jAnimator, Animator::FuncHandleAnimator callback)
{
    if (jAnimator == 0) {
        return;
    }
    intptr_t viewPtrInt = static_cast<intptr_t>(jAnimator);
    CJAnimator *cjAnimator = reinterpret_cast<CJAnimator *>(viewPtrInt);
    if (!cjAnimator) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieAnimator_handleAnimator", "cjAnimator is null.");
        return;
    }
    std::shared_ptr<Animator> animators = cjAnimator->getAnimator();
    if (!animators) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieAnimator_handleAnimator", "Animator is null.");
        return;
    }
    animators->g_jAnimator = jAnimator;
    animators->handleAnimator = callback;
}

void cj_LottieAnimator_setAnimatorFrameRate(const int64_t jAnimator, Animator::FuncSetAnimatorFrameRate callback)
{
    if (jAnimator == 0) {
        return;
    }
    intptr_t viewPtrInt = static_cast<intptr_t>(jAnimator);
    CJAnimator *cjAnimator = reinterpret_cast<CJAnimator *>(viewPtrInt);
    if (!cjAnimator) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieAnimator_setAnimatorFrameRate", "cjAnimator is null.");
        return;
    }
    std::shared_ptr<Animator> animators = cjAnimator->getAnimator();
    if (!animators) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieAnimator_setAnimatorFrameRate", "Animator is null.");
        return;
    }
    animators->g_jAnimator = jAnimator;
    animators->setAnimatorFrameRate = callback;
}

void cj_LottieAnimator_onFrame(const int64_t jAnimator)
{
    if (jAnimator == 0) {
        return;
    }
    intptr_t viewPtrInt = static_cast<intptr_t>(jAnimator);
    CJAnimator *cjAnimator = reinterpret_cast<CJAnimator *>(viewPtrInt);
    if (!cjAnimator) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieAnimator_onFrame", "cjAnimator is null.");
        return;
    }
    std::shared_ptr<Animator> animators = cjAnimator->getAnimator();
    if (!animators) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieAnimator_onFrame", "Animator is null.");
        return;
    }
    animators->OnCallBack();
}

}
}
