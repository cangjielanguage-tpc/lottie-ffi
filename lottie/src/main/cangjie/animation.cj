/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

package lottie

import ohos.display.*



@FastNative
foreign {

    func cj_LottieView_uniqueID(): CJBytesArray

    func cj_LottieView_createNativeRoot(lottieId: CString, config: LottieDataConfig): Unit

}

public enum ContentFIllMode  <: Equatable<ContentFIllMode>{
	|Fill
	|Top
	|Cover
	|Bottom
	|Contain

    public operator func ==(rhs: ContentFIllMode): Bool {
        match ((this, rhs)) {
            case (Fill, Fill) => true
            case (Top, Top) => true
            case (Cover, Cover) => true
            case (Bottom, Bottom) => true
            case (Contain, Contain) => true
            case _ => false
        }
    }

    public operator func !=(rhs: ContentFIllMode): Bool {
        return !(this == rhs)
    }
}

public struct AnimationConfig {

    /**
     * 应用内的唯一id
     * lottieId 用作控制动画的核心参数，默认可不设置，设置时要唯一，建议使用纯字母和数字
     * @example lottieId: 'lottie1'
     */
    public var lottieId: String

    /**
     * 应用内的动画数据文件路径, 仅限 json 格式
     * path 路径支持本地文件和网络资源
     * @example path: path: "https://xxx.com/xxxx.json"
     */
    public let path: String

    /**
     * json 格式的动画数据, 仅限 json 字符串格式，优先级高于 path
     */
    public let animationData: String

    /**
     * 是否循环播放/循环次数.
     * 默认值 -1, 值为 -1 时代表无限循环播放, 值 >=1 时为设置重复播放的次数
     * 定为 Int64 数据类型，< 0 表示无限循环播放；= 0 表示 不循环播放；> 0 为设置重复播放的次数。
     */
    public var loop: Int64

    /**
     * 自动播放设置
     */
    public var autoplay: Bool

    /**
     * 初始化动画资源播放时的整体帧范围: [1-60]
     */
    public var initialSegment: (Float64, Float64)

    /**
     * 动画填充模式 支持填充模式：Fill, Top, Cover, Bottom, Contain  默认模式是：Contain
     */
    public var contentMode: String

    /**
     * 设置 animator 的刷帧率，范围 1 ~ 120
     */
    public var frameRate: UInt8

    /**
     * 当动画不可见时，是否跳过绘制：设为 true 则跳过绘制，设为 false 则不跳过绘制(即无条件绘制)，默认值为true
     */
    public var autoSkip: Bool

    /**
     * 控制动画操作，通过 new LottieController()创建
     */
    /* LottieController 对应的 c 端 CJLottieController 对象的指针值。*/
    public var controller: Int64

    /**
     * 监听事件，通过 new LottieListener()创建
     */
    public let listener: LottieListener

    /**
     * 设置待替换的图像资源(url/base64)
     * 设置 Assets 资源替换功能，[图片资源 id，base64/uri]
     */
    public let setImageAssetDelegate: Array<(String, String)>

    /**
     * 当前设备的屏幕密度
     */
    public let density: Float64

    /**
     * 是否使用缓存，默认 true，使用缓存
     */
    public var useCache: Bool

    /* 设置 Xcomponent 的边框圆角 */
    public var radius: Float64

    /* cangjie 端利用 @ohos.animator 和 c 交互，c 端 Animator 对象的指针值。*/
    public var animator: Int64

    public var visible: Bool = false
    public var runOnce: Bool = false

    public init(
        lottieId!: String = "",
        path!: String = "",
        radius!: Float64 = 0.0,
        animationData!: String = "",
        loop!: Int64 = -1,
        autoplay!: Bool = true,
        initialSegment!: (Float64, Float64) = (0.0, 0.0),
        contentMode!: String = "Contain",
        frameRate!: UInt8 = 30,
        useCache!: Bool = true,
        autoSkip!: Bool = true,
        controller!: Int64 = 0,
        listener!: LottieListener = LottieListener(),
        density!: Float64 = 0.0,
        setImageAssetDelegate!: Array<(String, String)> = Array<(String, String)>(),
        animator!: Int64 = 0
    ) {
        this.loop = loop
        this.autoplay = autoplay
        this.autoSkip = autoSkip
        this.lottieId = lottieId
        this.path = path
        this.animationData = animationData
        this.initialSegment = initialSegment
        this.contentMode = contentMode
        this.frameRate = frameRate
        this.listener = listener
        this.density = density
        this.controller = controller
        this.setImageAssetDelegate = setImageAssetDelegate
        this.useCache = useCache
        this.radius = radius
        this.animator = animator
    }

    func toLottieDataConfig(): LottieDataConfig {
        unsafe {
            let segment: VArray<Float64, $2> = [this.initialSegment[0], this.initialSegment[1]]

            let len: Int64 = this.setImageAssetDelegate.size
            let segments: Array<ImageAssetDelegate>
            if (len > 0) {
                let item1: CString = LibC.mallocCString(this.setImageAssetDelegate[0][0])
                let item2: CString = LibC.mallocCString(this.setImageAssetDelegate[0][1])
                let empty_item: ImageAssetDelegate = ImageAssetDelegate(item1, item2)
                segments = Array<ImageAssetDelegate>(len, repeat: empty_item)
                for (i in 1..len) {
                    let res: (String, String) = this.setImageAssetDelegate[i]
                    segments[i] = ImageAssetDelegate(LibC.mallocCString(res[0]), LibC.mallocCString(res[1]))
                }
            } else {
                segments = Array<ImageAssetDelegate>()
            }
            let handleSegments: CPointerHandle<ImageAssetDelegate> = acquireArrayRawData(segments)
            return LottieDataConfig(
                lottieId: LibC.mallocCString(this.lottieId),
                loop: this.loop,
                autoplay: this.autoplay,
                autoSkip: this.autoSkip,
                path: LibC.mallocCString(this.path),
                animationData: LibC.mallocCString(this.animationData),
                initialSegment: segment,
                contentMode: LibC.mallocCString(this.contentMode),
                frameRate: this.frameRate,
                listener: LottieListenerConfig(this.listener),
                density: this.density,
                controller: this.controller,
                setImageAssetDelegate: handleSegments.pointer,
                setImageAssetDelegateSize: len,
                useCache: this.useCache,
                animator: this.animator
            )
        }
    }
}


public func getUniqueId(): String {
    unsafe {
        let res: CJBytesArray = cj_LottieView_uniqueID()
        let id: String = BytesArrayToString(res)
        free_CJBytesArray(res)
        return id
    }
}


/**
 * 创建Native的UI组件
 *
 * @param { string } lottieId - 动画ID
 * @param { Object } config - 动画配置
 */
func createNativeRoot(lottieId: String, config: AnimationConfig): Unit {
    unsafe {
        let id: CString = LibC.mallocCString(lottieId)
        let c_config: LottieDataConfig = config.toLottieDataConfig()
        cj_LottieView_createNativeRoot(id, c_config)
        LibC.free(id)
    }
}


/**
 * 设置组件当前是否可见
 * @param { string } lottieId - 动画ID
 * @param { boolean } visible - 是否可见
 */
//TODO:lipei-----------------------------------[未实现]-----------------------------------------------
func lottieSetVisible(lottieId: String, visible: Bool): Unit {}



