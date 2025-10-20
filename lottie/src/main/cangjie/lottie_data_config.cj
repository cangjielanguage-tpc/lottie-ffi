/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

package lottie

@C
public struct ImageAssetDelegate {
    public var refId: CString
    public var fileName: CString

    public init(refId: CString, fileName: CString) {
        this.refId = refId
        this.fileName = fileName
    }
}

@C
public struct AnimationSegment {
    public var initFrame: Float32
    public var endFrame: Float32

    public init(initFrame: Float32, endFrame: Float32) {
        this.initFrame = initFrame
        this.endFrame = endFrame
    }
}

@C
public struct LottieDataConfig {

    /**
     * 应用内的唯一id
     * lottieId 用作控制动画的核心参数，默认可不设置，设置时要唯一，建议使用纯字母和数字
     * @example lottieId: 'lottie1'
     */
    public var lottieId: CString

    /**
     * 应用内的动画数据文件路径, 仅限json格式
     * path 路径支持本地文件和网络资源
     * @example path: $rawfile('common/lottie/animation.json')
     * @example path: path: json 文件的 uri 下载链接
     */
    //public let path?: String | Resource
    public let path: CString

    /**
     * json格式的动画数据, 仅限json字符串格式
     */
    public let animationData: CString

    /**
     * 动画播放结束后, 是否循环播放，默认值true, 值为true时无限循环播放 值类型为number, 且>=1时为设置重复播放的次数
     */
    //public var loop: (Bool, UInt64)
    /**
     * 动画播放结束后, 是否循环播放。默认值 -1, 值为 -1 时代表无限循环播放, 值 >=1 时为设置重复播放的次数
     * 定为 Int64 数据类型，< 0 表示无限循环播放；= 0 表示 不循环播放；> 0 为设置重复播放的次数。
     */
    public var loop: Int64 = -1

    /**
     * 自动播放设置
     */
    public var autoplay: Bool = false

    /**
     * 初始化动画资源播放时的整体帧范围
     */
    public var initialSegment: VArray<Float64, $2>

    /**
     * 动画填充模式 支持填充模式：Fill, Top, Cover, Bottom, Contain  默认模式是：Contain
     */
    public var contentMode: CString //= "Contain"

    /**
     * 设置 animator 的刷帧率，范围 1 ~ 120
     */
    public var frameRate: UInt8 = 30

    /**
     * 当动画不可见时，是否跳过绘制：设为true则跳过绘制，设为false则不跳过绘制(即无条件绘制)，默认值为true
     */
    public var autoSkip: Bool = true

    /**
     * 控制动画操作
     */
    /* LottieController 对应的 c 端 CJLottieController 对象的指针值。*/
    public var controller: Int64

    /**
     * 监听事件
     */
    public let listener: LottieListenerConfig

    /**
     * 设置待替换的图像资源(url/base64)
     */
    public let setImageAssetDelegate: CPointer<ImageAssetDelegate>
    public let setImageAssetDelegateSize: Int64

    /**
     * 当前设备的屏幕密度
     */
    public let density: Float64

    /**
     * 设置是否使用缓存，默认为 true
     */
    public var useCache: Bool = true

    /**
     * cangjie 端利用 @ohos.animator 和 c 交互，c 端 Animator 对象的指针值。
     */
    public let animator: Int64

    public init(
        lottieId!: CString,
        loop!: Int64,
        autoplay!: Bool,
        autoSkip!: Bool,
        path!: CString,
        animationData!: CString,
        initialSegment!: VArray<Float64, $2>,
        contentMode!: CString,
        frameRate!: UInt8,
        listener!: LottieListenerConfig,
        density!: Float64,
        controller!: Int64,
        setImageAssetDelegate!: CPointer<ImageAssetDelegate>,
        setImageAssetDelegateSize!: Int64,
        useCache!: Bool,
        animator!: Int64
    ) {
        this.lottieId = lottieId
        this.loop = loop
        this.autoplay = autoplay
        this.autoSkip = autoSkip
        this.path = path
        this.animationData = animationData
        this.initialSegment = initialSegment
        this.contentMode = contentMode
        this.frameRate = frameRate
        this.listener = listener
        this.density = density
        this.controller = controller
        this.setImageAssetDelegate = setImageAssetDelegate
        this.setImageAssetDelegateSize = setImageAssetDelegateSize
        this.useCache = useCache
        this.animator = animator
    }
}
