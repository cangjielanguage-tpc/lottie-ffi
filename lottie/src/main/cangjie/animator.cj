/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

package lottie

import kit.UIKit.*

internal import kit.BasicServicesKit.SystemDateTime

internal import std.collection.HashMap

@FastNative
foreign {

    func cj_LottieAnimator_createLottieAnimator(): Int64

    func cj_LottieAnimator_handleAnimator(jView: Int64, cb: CFunc<(UInt8, Int64) -> Unit>): Unit

    func cj_LottieAnimator_setAnimatorFrameRate(jView: Int64, cb: CFunc<(Int64, Float64) -> Unit>): Unit

    func cj_LottieAnimator_onFrame(jView: Int64): Unit

}

private var controllerMap: HashMap<Int64, Option<LottieAnimator>> = HashMap<Int64, Option<LottieAnimator>>()

/**
 * Sets the expected frame rate range for the animation of an animator.
 */
@C
func setAnimatorFrameRate(jView: Int64, frameRatesNew: Float64): Unit {
    if (controllerMap.contains(jView)) {
        controllerMap[jView].getOrThrow().setFrameRate(frameRatesNew)
    }
}

/**
 * handle the animator.
 * state - 0: create - create an Animator.
 * state - 1: cancel - cancel the Animator.
 * state - 2: play   - Starts the animation of an animator.
 * state - 3: pause  - Pauses the animation of an animator.
 * state - 4: stop   - Ends the animation of an animator.
 */
@C
func handleAnimator(state: UInt8, jView: Int64): Unit {
    if (controllerMap.contains(jView)) {
        match (state) {
            case 0 =>
                //controllerMap[jView].getOrThrow().create()
            case 1 =>
                controllerMap[jView].getOrThrow().cancel()
            case 2 =>
                controllerMap[jView].getOrThrow().play()
            case 3 =>
                controllerMap[jView].getOrThrow().pause()
            case 4 =>
                controllerMap[jView].getOrThrow().stop()
            case _ => ()
        }
    }
}


public class LottieAnimator {

    var frameRate: Float64 = 0.0
    var _frameRate: Bool = false

    //需要引入 @ohos.animator 的 AnimatorResult
    var _animator: ?AnimatorResult = None

    // Binding with C code, the object pointer returned by C code.
    public var ptr: Int64 = 0


    public init() {
        unsafe {
            ptr = cj_LottieAnimator_createLottieAnimator()
            cj_LottieAnimator_handleAnimator(ptr, handleAnimator)
            cj_LottieAnimator_setAnimatorFrameRate(ptr, setAnimatorFrameRate)
        }
        if (controllerMap.contains(ptr)) {
            controllerMap[ptr] = this
        } else {
            controllerMap.add(ptr, this)
        }

        if (let Some(v) <- this._animator) {
            return
        }

        let options: AnimatorOptions = AnimatorOptions(
            duration: 20000,
            easing: "linear",
            delay: 0,
            fill: AnimatorFill.Forwards, //"forwards",
            direction: AnimatorDirection.Normal, //"normal",
            iterations: -1,
            begin: 0.0,
            end: 1.0
        )

        let backAnimator: AnimatorResult = AnimatorResult(options)

        backAnimator.onFrame = {
            value: Float64  =>
                resume()
        }
        let callBackGet = backAnimator.onFrame
        callBackGet(666.0)

        this._animator = backAnimator
    }

    func play(): Unit {
        if (let Some(v) <- this._animator) {
            v.play()
        }
    }

    func pause(): Unit {
        if (let Some(v) <- this._animator) {
            v.pause()
        }
    }

    func stop(): Unit {
        if (let Some(v) <- this._animator) {
            v.finish()
        }
    }

    func cancel(): Unit {
        if (let Some(v) <- this._animator) {
            v.cancel()
        }
    }

    func reset(options: AnimatorOptions): Unit {
        if (let Some(v) <- this._animator) {
            v.reset(options)
        }
    }

    func setFrameRate(frameRatesNew: Float64) {
        var frameRates: Float64 = frameRatesNew
        if (frameRates <= 0.0 || frameRates > 120.0) {
            frameRates = 30.0
        }
        frameRate = frameRates

        if (frameRate > 0.0 && frameRate <= 120.0) {
            var expectedFrameRate = ExpectedFrameRateRange(
                min: 1,
                max: 120,
                expected: Int32(frameRate)
            )

            if (let Some(v) <- this._animator) {
                v.setExpectedFrameRateRange(expectedFrameRate)
            }
        }
    }


    /**
     * 帧回调函数，改成调用 c 代码中的 Animator::OnCallBack。
     * 底层调用的是 Animator.FunctionCallBack 成员变量
     * Animator.FunctionCallBack 是在 LottieHandler::SetAnimatorCallBack 函数中赋值的
     * 最底层调用的是 LottieXcomponentNode::RenderAndAdvanceFrame 函数
     */
    func resume(): Unit {
        unsafe {
            cj_LottieAnimator_onFrame(ptr)
        }
    }
}
