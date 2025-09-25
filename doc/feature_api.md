# lottie-turbo 特性文档

## 介绍

lottie-turbo 是一个动画库，它可以解析Adobe After Effects软件通过Bodymovin插件导出的json格式的动画，并在移动设备上进行本地渲染。


## 1 LottieView 组件控制器

### 1.1 LottieView Interface API Description


```cangjie
@Component
public class LottieView {

    /* 应用内的唯一id */
    var lottieId: String

    /* 应用内的动画数据文件路径, 仅限json格式  */
    var path: String

    /* json格式的动画数据, 仅限json字符串格式，优先级高于path */
    var animationData: String
    
    /**
     * 是否循环播放/循环次数
     * 定为 Int64 数据类型，< 0 表示无限循环播放；= 0 表示 不循环播放；> 0 为设置重复播放的次数。
     */
    var loop: Int64

    /* 是否自动播放 */
    var autoplay: Bool

    /* 初始化动画资源播放时的整体帧范围:[1-60] */
    var initialSegment: (Float64, Float64)
    
    /* 动画填充模式，取值 Fill,Cover,Top,Bottom,Contain  */
    var contentMode: String

    /* 设置animator的刷帧率，范围1~120 */
    var frameRate: UInt8

    /* 是否使用缓存，默认true，使用缓存 */
    var useCache: Bool
    
    /* 当动画不可见时，是否跳过绘制：设为true则跳过绘制，默认为 true */
    var autoSkip: Bool

    /* 控制动画操作，通过 LottieController()创建 */
    var controller: LottieController

    /* 监听事件，通过new LottieListener()创建 */
    var listener: Option<LottieListener>
    
    /* 设置Assets资源替换功能，[图片资源id，base64/uri] */
    var setImageAssetDelegate: Array<(String, String)>
}
```

## 2 LottieController 单个动画控制器

### 2.1 LottieController Class API Description

```cangjie
public class LottieController {

    /**
     * LottieController 的构造方法
     */
    public init() 
    
    /* 控制动画唯一id */
    public mut prop lottieId: String
    
    /* 动画是否加载 */
    public prop isLoaded: : Option<Bool>
    
    /* 动画当前帧号，默认为浮点数，调用setSubframe(false) 后为整数 */
    public prop currentFrame: Option<Float64>
    
    /* 动画当前帧数 */
    public prop currentRawFrame: Option<Float64>
    
    /* 当前动画播放片段的第一帧帧号  */
    public prop firstFrame: Option<Float64>
    
    /* 当前动画播放片段的总帧数 */
    public prop totalFrames: Option<Float64>
    
    /* 动画帧率 frame/s  */
    public prop frameRate: Option<Float64>
    
    /* 动画帧率 frame/ms */
    public prop frameMult: Option<Float64>
    
    /* 动画播放速度 */
    public prop playSpeed: Option<Float64>
    
    /* 动画播放方向，1 正向播放；-1 反向播放，0 获取播放方向失败。 */
    public prop playDirection: Option<Int64>
    
    /* 动画完成播放的次数 */
    public prop playCount: Option<Int64>
    
    /* 动画是否已暂停 */
    public prop isPaused: Option<Bool>
    
    /* 动画是否自动播放 */
    public prop autoplay: Option<Bool>
    
    /**
     * 动画播放结束后, 是否循环播放。默认值 -1, 值为 -1 时代表无限循环播放, 值 >=1 时为设置重复播放的次数
     * 定为 Int64 数据类型，< 0 表示无限循环播放；= 0 表示 不循环播放；> 0 为设置重复播放的次数。
     */
    public prop loop: Option<Int64>
    
    /* 当前动画片段完成单次播放的帧数 */
    public prop timeCompleted: Option<Float64>
    
    /* 当前动画片段序号 */
    public prop segmentPos: Option<Int64>
    
    /* 是否尽可能更新动画帧率 */
    public prop isSubframeEnabled: Option<Bool>
    
    /* 当前动画播放片段 */
    public prop segments: Array<(Float64, Float64)>
    
    /**
     * 动画播放
     */
    public func play()
    
    /**
     * 动画停止
     */
    public func stop()
    
    /**
     * 动画暂停播放切换
     */
    public func togglePause()
    
    /**
     * 动画销毁
     */
    public func destroy()
    
    /**
     * 动画暂停
     */
    public func pause()
    
    /**
     * 控制动画画面停止在某一帧或某个时刻
     * 参数 value - Float64类型,帧号(值>=0)或时刻(ms)
     * 参数 isFrame - ?Bool类型,true按帧控制, false按时间控制, 缺省默认false
     */
    public func goToAndStop(value: Float64, isFrame: ?Bool)
    
    /**
     * 控制动画画面从在某一帧或某个时刻开始播放
     * 参数 value - Float64类型, 帧号(>=0)或时刻(ms)
     * 参数 isFrame - ?Bool类型, true按帧控制, false按时间控制, 缺省默认false
     */
    public func goToAndPlay(value: Float64, isFrame: ?Bool)
    
    /**
     * 限定动画资源播放时的整体帧范围
     * 参数 start - Float64类型, 起始帧号
     * 参数 end - Float64类型, 结束帧号
     */
    public func setSegment(start: Float64, end: Float64)
    
    /**
     * 设置仅播放指定范围的帧动画
     * 参数 segments - Array<(Float32, Float32)>类型, 片段或片段数组 若传入的是数组, 且当前loop!=0, 播放结束后, 仅循环播放最后一个片段
     * 参数 forceFlag - ?Bool类型, 值为true立刻生效, 值为false循环下次播放的时候生效
     */
    public func playSegments(segments: Array<(Float32, Float32)>, forceFlag: ?Bool)
    
    /**
     * 重置动画播放片段, 使动画重新从第一帧开始播放完整动画
     * 参数 forceFlag - Bool类型, 值为true立刻生效, 值为false循环下次播放的时候生效
     */
    public func resetSegments(forceFlag: Bool)
    
    /**
     * 设置播放速度
     * 参数 speed - Float64类型, 值为浮点类型, speed>0正向播放, speed<0反向播放, speed=0暂停播放, speed=1.0/-1.0正常速度播放
     */
    public func setSpeed(speed: Float64)
    
    /**
     * 设置播放方向
     * 参数 direction - Int64类型, 1为正向, -1为反向
     */
    public func setDirection(direction: Int64)
    
    /**
     * 设置尽可能的更新动画帧率
     * 参数 useSubFrames - Bool类型, true为使用子帧, false为不使用子帧
     * @return Bool类型,返回 true 代表执行成功，返回 false 代表执行失败.
     */
    public func setSubframe(useSubFrames: Bool): Bool
    
    /**
     * 获取动画播放时间
     * 参数 inFrames - ?Bool类型, 为 true 时获取帧数，值为 false 时获取时间(单位ms)，默认为 false。
     * @return Option<Float64>类型, 返回的帧数或者时间值，为 None 时代表获取失败。
     */
    public func getDuration(inFrames: ?Bool): Option<Float64>
    
    /**
     * 强制触发指定事件
     * 参数 name - String类型, 事件名称 'enterFrame' | 'loopComplete' | 'complete' | 'segmentStart' | 'destroy' | 'config_ready' | 'data_ready' | 'DOMLoaded' | 'error' | 'data_failed' | 'loaded_images'
     */
    public func triggerEvent(name: String)
    
    /**
     * 添加监听事件
     * 参数 name - String类型, 事件名称
     * 参数 callback 事件回调函数
     */
    public func addEventListener(name: String, callback: CFunc<() -> Unit>)
    
    /**
     * 移除监听事件
     * 参数 name - String类型, 事件名称
     * 参数 callback 移除后触发的事件回调函数
     */
    public func removeEventListener(name: String, callback: Option<CFunc<() -> Unit>>)
    
    /**
     * 改变动画某个图层的颜色，当前支持st和fl图层
     * 参数 layer - String类型, 图层名称
     * 参数 startColor - Array<Float64>类型, 起始颜色，若endColor不传，则startColor为要修改的颜色，支持RGB和RGBA格式
     * 参数 endColor - ?Array<Float64>类型, 结束颜色，支持RGB和RGBA格式
     */
    public func changeColor(layer: String, startColor: Array<Float64>, endColor: ?Array<Float64>)
    
    /**
     * 设置布局模式
     * @param contentMode - String类型, 布局模式，Fill,Cover,Top,Bottom,Contain，默认为 Contain
     */
    public func setContentMode(contentMode: String)
    
    /**
     * 设置动画帧率
     * 参数 frameRate - Float64类型, 动画帧率，范围1~120
     */
    public func setFrameRate(frameRate: Float64)
    
    /**
     * 设置动画重载
     * 参数 config - AnimationConfig类型, 动画配置
     */
    public func reload(config: AnimationConfig)
    
}
```


## 3 LottieSource 全局动画控制器

### 3.1 LottieSource Class API Description

```cangjie
public class LottieSource {
    
    /**
     * 播放动画
     * 参数 lottieId - String类型, 动画唯一id
     */
    public func play(lottieId!: String = "")
    
    /**
     * 暂停动画
     * 参数 lottieId - String类型, 动画唯一id
     */
    public func pause(lottieId!: String = "")
    
    /**
     * 全局动画暂停与播放切换
     * 参数 lottieId - String类型, 动画唯一id
     */
    public func togglePause(lottieId!: String = "")
    
    /**
     * 停止动画
     * 参数 lottieId - String类型, 动画唯一id
     */
    public func stop(lottieId!: String = "")
    
    /**
     * 全局动画播放方向控制
     * 参数 speed - Float64类型, 全局动画播放方向控制，speed>0正向播放, speed<0反向播放, speed=0暂停播放, speed=1.0/-1.0正常速度播放
     * 参数 lottieId - String类型, 动画唯一id
     */
    public func setSpeed(speed: Float64,lottieId!: String = "")
    
    /**
     * 全局设置播放方向
     * 参数 direction - Float64类型,1.0为正向, -1.0为反向
     * 参数 lottieId - String类型, 动画唯一id
     */
    public func setDirection(direction: Float64,lottieId!: String = "")
    
    /**
     * 销毁动画
     * 参数 lottieId - String类型, 动画唯一id
     */
    public func destroy(lottieId!: String = "")
    
    /**
     * 全局设置播放帧率
     * 参数 frameRate - Float64类型, 播放帧率
     */
    public func setFrameRate(frameRate: Float64)
    
    /**
     * 全局文件缓存清除
     * 参数 lottieId - String类型, 动画唯一id
     */
    public func clearFileCache(url!: String = "")
    
    /**
     * 设置文件缓存容器大小
     * 参数 size - Int64类型, 缓存文件的个数
     * 参数 capacity - Int64类型, 缓存文件的总大小
     */
    public func resizeFileCache(size: Int64, capacity: Int64)
    
    /**
     * 设置缓存容器大小
     * 参数 size - Int64类型, 缓存容器可以动画个数
     * 参数 capacity - Int64类型, 缓存容器的总内存大小
     */
    public func resizeCache(size: Int64, capacity: Int64)
    
    /**
     * 清空缓存容器
     */
    public func clearCache()
    
    /**
     * 通过path移除指定缓存
     * 参数 path - String类型, 动画唯一id
     */
    public func removeCache(path: String)

    /**
     * 将lottie动画的某帧离屏渲染为图像数据
     * 参数 lottie - String类型, 动画唯一id
     * 参数 frameNum - Float64类型, 帧序号
     * 参数 width - Float64类型, 宽度
     * 参数 height - Float64类型, 高度
     * @return CPointer<ArrayBuffer>类型
     */
    public func renderToImage(lottie: String,frameNum: Float64, width: Float64, height: Float64): CPointer<ArrayBuffer>
    
}
```