/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

package lottie

@C
public struct LottieListenerConfig {

    var onDrawFrame: CFunc<() -> Unit>
    var onEnterFrame: CFunc<() -> Unit>
    var onLoopComplete: CFunc<() -> Unit>
    var onComplete: CFunc<() -> Unit>
    var onSegmentStart: CFunc<() -> Unit>
    var onDestroy: CFunc<() -> Unit>
    var onConfigReady: CFunc<() -> Unit>
    var onDataReady: CFunc<() -> Unit>
    var onDOMLoaded: CFunc<() -> Unit>
    var onDataFailed: CFunc<() -> Unit>
    var onLoadedImages: CFunc<() -> Unit>

    public init(
        onDrawFrame!: CFunc<() -> Unit> = { => () },
        onEnterFrame!: CFunc<() -> Unit> = { => () },
        onLoopComplete!: CFunc<() -> Unit> = { => () },
        onComplete!: CFunc<() -> Unit> = { => () },
        onSegmentStart!: CFunc<() -> Unit> = { => () },
        onDestroy!: CFunc<() -> Unit> = { => () },
        onConfigReady!: CFunc<() -> Unit> = { => () },
        onDataReady!: CFunc<() -> Unit> = { => () },
        onDOMLoaded!: CFunc<() -> Unit> = { => () },
        onDataFailed!: CFunc<() -> Unit> = { => () },
        onLoadedImages!: CFunc<() -> Unit> = { => () }
    ) {
        this.onDrawFrame = onDrawFrame
        this.onEnterFrame = onEnterFrame
        this.onLoopComplete = onLoopComplete
        this.onComplete = onComplete
        this.onSegmentStart = onSegmentStart
        this.onDestroy = onDestroy
        this.onConfigReady = onConfigReady
        this.onDataReady = onDataReady
        this.onDOMLoaded = onDOMLoaded
        this.onDataFailed = onDataFailed
        this.onLoadedImages = onLoadedImages
    }

    public init(options: LottieListener) {
        this.onDrawFrame = options.onDrawFrame
        this.onEnterFrame = options.onEnterFrame
        this.onLoopComplete = options.onLoopComplete
        this.onComplete = options.onComplete
        this.onSegmentStart = options.onSegmentStart
        this.onDestroy = options.onDestroy
        this.onConfigReady = options.onConfigReady
        this.onDataReady = options.onDataReady
        this.onDOMLoaded = options.onDOMLoaded
        this.onDataFailed = options.onDataFailed
        this.onLoadedImages = options.onLoadedImages
    }
}

public class LottieListener {

    var onDrawFrame: CFunc<() -> Unit>
    var onEnterFrame: CFunc<() -> Unit>
    var onLoopComplete: CFunc<() -> Unit>
    var onComplete: CFunc<() -> Unit>
    var onSegmentStart: CFunc<() -> Unit>
    var onDestroy: CFunc<() -> Unit>
    var onConfigReady: CFunc<() -> Unit>
    var onDataReady: CFunc<() -> Unit>
    var onDOMLoaded: CFunc<() -> Unit>
    var onDataFailed: CFunc<() -> Unit>
    var onLoadedImages: CFunc<() -> Unit>

    public init(
        onDrawFrame!: CFunc<() -> Unit> = { => return () },
        onEnterFrame!: CFunc<() -> Unit> = { => return () },
        onLoopComplete!: CFunc<() -> Unit> = { => return () },
        onComplete!: CFunc<() -> Unit> = { => return () },
        onSegmentStart!: CFunc<() -> Unit> = { => return () },
        onDestroy!: CFunc<() -> Unit> = { => return () },
        onConfigReady!: CFunc<() -> Unit> = { => return () },
        onDataReady!: CFunc<() -> Unit> = { => return () },
        onDOMLoaded!: CFunc<() -> Unit> = { => return () },
        onDataFailed!: CFunc<() -> Unit> = { => return () },
        onLoadedImages!: CFunc<() -> Unit> = { => return () }
    ) {
        this.onDrawFrame = onDrawFrame
        this.onEnterFrame = onEnterFrame
        this.onLoopComplete = onLoopComplete
        this.onComplete = onComplete
        this.onSegmentStart = onSegmentStart
        this.onDestroy = onDestroy
        this.onConfigReady = onConfigReady
        this.onDataReady = onDataReady
        this.onDOMLoaded = onDOMLoaded
        this.onDataFailed = onDataFailed
        this.onLoadedImages = onLoadedImages
    }
}
