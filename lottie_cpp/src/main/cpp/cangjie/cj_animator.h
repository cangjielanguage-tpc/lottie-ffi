/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef RLOTTIE510828_CJ_ANIMATOR_H
#define RLOTTIE510828_CJ_ANIMATOR_H

#include <atomic>
#include <cstdint>
#include <string>
#include <memory>
#include <iostream>
#include <vector>

#include <hilog/log.h>

#include "animator.h"

extern "C" {
namespace LottieTurbo {
class CJAnimator {
public:
    explicit CJAnimator(std::shared_ptr<Animator> animator) : animator(animator) {}

    ~CJAnimator();

    std::shared_ptr<Animator> getAnimator()
    { 
        std::lock_guard<std::mutex> autoLock(locker);
        return animator; 
    }

    void clear()
    { 
        std::lock_guard<std::mutex> autoLock(locker);
        animator = nullptr;
    }

private:
    std::shared_ptr<Animator> animator;
    std::mutex locker;
};

int64_t cj_LottieAnimator_createLottieAnimator();
/**
 * handle the animator.
 * state - 0: create - create an Animator.
 * state - 1: cancel - cancel the Animator.
 * state - 2: play   - Starts the animation of an animator.
 * state - 3: pause  - Pauses the animation of an animator.
 * state - 4: stop   - Ends the animation of an animator.
 */
void cj_LottieAnimator_handleAnimator(const int64_t jAnimator, Animator::FuncHandleAnimator callback);
void cj_LottieAnimator_setAnimatorFrameRate(const int64_t jAnimator, Animator::FuncSetAnimatorFrameRate callback);
void cj_LottieAnimator_onFrame(const int64_t jAnimator);

};
};

#endif //RLOTTIE510828_CJ_ANIMATOR_H
