/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef RLOTTIE510828_ANIMATOR_H
#define RLOTTIE510828_ANIMATOR_H

#include <arkui/native_animate.h>
#include <cstdint>
#include <string>

#include "hilog/log.h"

extern "C" {
namespace LottieTurbo {
class Animator {
public:
    enum class AnimatorStatus {
        IDLE,
        INIT,
        PLAY,
        STOP,
        END,
        PAUSE
    };
    ~Animator();
    void SetDuration(int32_t duration);
    int32_t SetDelay(int32_t delay);
    int32_t SetIterations(int32_t);
    int32_t SetBegin(float begin);
    int32_t SetEnd(float end);
    int32_t SetEasing(ArkUI_AnimationCurve easing);
    int32_t SetDirection(ArkUI_AnimationDirection direction);
    int32_t SetFill(ArkUI_AnimationFillMode fill);

    /**
     * @brief 保存frameRateRange
     * @param frameRateRange 帧率范围
     */
    void SetFrameRateRange(ArkUI_ExpectedFrameRateRange frameRateRange);

    /**
     * @brief 设置frameRateRange给ArkUI
     * @return Returns the error code.
     *         Returns {@link ARKUI_ERROR_CODE_NO_ERROR} if the operation is successful.
     *         Returns {@link ARKUI_ERROR_CODE_PARAM_INVALID} if a parameter error occurs.
     */
    void UpdateFrameRateRange();

    void Init();
    std::function<void()> FunctionCallBack;
    void Play();
    void Pause();
    void Stop();
    const AnimatorStatus GetStatus() const;
    
    void OnCallBack();
    
    typedef void (*FuncHandleAnimator)(uint8_t, int64_t);
    FuncHandleAnimator handleAnimator;

    typedef void (*FuncSetAnimatorFrameRate)(int64_t, double);
    FuncSetAnimatorFrameRate setAnimatorFrameRate;
    int64_t g_jAnimator = 0;

private:

    ArkUI_ExpectedFrameRateRange frameRateRange_{.min = 0, .max = 0, .expected = 0};
    std::atomic<AnimatorStatus> status_{Animator::AnimatorStatus::IDLE};
    
    /**
     * handle the animator.
     * state - 0: create - create an Animator.
     * state - 1: cancel - cancel the Animator.
     * state - 2: play   - Starts the animation of an animator.
     * state - 3: pause  - Pauses the animation of an animator.
     * state - 4: stop   - Ends the animation of an animator.
     */
    uint8_t createAnimator = 0;
    uint8_t cancelAnimator = 1;
    uint8_t playAnimator = 2;
    uint8_t pauseAnimator = 3;
    uint8_t stopAnimator = 4;
};

};
};

#endif //RLOTTIE510828_ANIMATOR_H
