/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

package lottie

import ohos.state_macro_manage.*
import std.convert.*
import std.sync.*


@FastNative
foreign {

    func cj_LottieController_init(): Int64
    func cj_LottieController_onSurfaceCreated(jView: Int64, surfaceId: Int64, surfaceWidth: UInt64, surfaceHeight: UInt64): Unit
    func cj_LottieController_onSurfaceChanged(jView: Int64, surfaceId: Int64, surfaceWidth: UInt64, surfaceHeight: UInt64): Unit

    func cj_LottieController_getId(ptr: Int64): CJBytesArray
    func cj_LottieController_setId(ptr: Int64, lottieId: CString): Bool
    func cj_LottieController_isLoaded(ptr: Int64): CJBoolStruct
    func cj_LottieController_get_currentFrame(ptr: Int64): CJFloat64Struct
    func cj_LottieController_set_currentFrame(ptr: Int64, value: Float64): Unit
    func cj_LottieController_get_currentRawFrame(ptr: Int64): CJFloat64Struct
    func cj_LottieController_set_currentRawFrame(ptr: Int64, value: Float64): Unit
    func cj_LottieController_get_firstFrame(ptr: Int64): CJFloat64Struct
    func cj_LottieController_get_totalFrames(ptr: Int64): CJFloat64Struct
    func cj_LottieController_get_frameRate(ptr: Int64): CJFloat64Struct
    func cj_LottieController_get_frameMult(ptr: Int64): CJFloat64Struct
    func cj_LottieController_get_playSpeed(ptr: Int64): CJFloat64Struct
    func cj_LottieController_get_playDirection(ptr: Int64): CJInt64Struct
    func cj_LottieController_get_playCount(ptr: Int64): CJInt64Struct
    func cj_LottieController_get_isPaused(ptr: Int64): CJBoolStruct
    func cj_LottieController_get_autoplay(ptr: Int64): CJBoolStruct
    func cj_LottieController_get_loop(ptr: Int64): CJInt64Struct
    func cj_LottieController_get_timeCompleted(ptr: Int64): CJFloat64Struct
    func cj_LottieController_get_segmentPos(ptr: Int64): CJInt64Struct
    func cj_LottieController_get_isSubframeEnabled(ptr: Int64): CJBoolStruct
    func cj_LottieController_get_segments(ptr: Int64): CJAnimationSegmentArray


    func cj_LottieController_play(ptr: Int64): Unit
    func cj_LottieController_stop(ptr: Int64): Unit
    func cj_LottieController_togglePause(ptr: Int64): Unit
    func cj_LottieController_destroy(ptr: Int64): Unit

    func cj_LottieController_pause(ptr: Int64): Unit
    func cj_LottieController_goToAndStop(ptr: Int64, value: Float64, isFrame: Bool): Unit
    func cj_LottieController_goToAndPlay(ptr: Int64, value: Float64, isFrame: Bool): Unit
    func cj_LottieController_setSegment(ptr: Int64, start: Float64, end: Float64): Unit
    func cj_LottieController_playSegments(ptr: Int64, segments: CPointer<AnimationSegment>, len: Int64, forceFlag: Bool, hasForceFlag: Bool): Unit

    func cj_LottieController_resetSegments(ptr: Int64, forceFlag: Bool): Unit
    func cj_LottieController_setSpeed(ptr: Int64, speed: Float64): Unit
    func cj_LottieController_setDirection(ptr: Int64, direction: Int64): Unit
    func cj_LottieController_setSubframe(ptr: Int64, useSubFrames: Bool): Bool
    func cj_LottieController_getDuration(ptr: Int64, isFrame: Bool): CJFloat64Struct
    func cj_LottieController_triggerEvent(ptr: Int64, names: CString): Unit
    func cj_LottieController_addEventListener(ptr: Int64, names: CString, callback: CFunc<() -> Unit>): Unit
    func cj_LottieController_removeEventListener(ptr: Int64, name: CString, callback: CFunc<() -> Unit>, hasCallbackFunc: Bool): Unit

    func cj_LottieController_changeColor(ptr: Int64, layer: CString, startColor: CPointer<Float64>,
        startColorLen: Int64, endColor: CPointer<Float64>, endColorLen: Int64): Unit
    func cj_LottieController_setContentMode(ptr: Int64, contentMode: CString): Unit
    func cj_LottieController_setFrameRate(ptr: Int64, frameRate: Float64): Unit
    func cj_LottieController_reload(ptr: Int64, config: LottieDataConfig): Unit

    func cj_LottieController_release(ptr: Int64): Unit

}

public class LottieController <: XComponentController {

    var isNull: Bool = true

    private var mt = Mutex()

    // Binding with C code, the object pointer returned by C code.
    private var ptr: Int64 = 0

    private var animator: LottieAnimator

    public init() {
        unsafe {
            let native: Int64 = cj_LottieController_init()
            if (native <= 0) {
                cj_LottieController_release(native)
            } else {
                this.ptr = native
            }
        }
        this.animator = LottieAnimator()
    }

    protected override func onSurfaceCreated(surfaceId: String) {
        unsafe {
            let rect: SurfaceRect = getXComponentSurfaceRect()
            cj_LottieController_onSurfaceCreated(ptr, Int64.parse(surfaceId), UInt64(rect.surfaceWidth), UInt64(rect.surfaceHeight))
        }
    }

    protected override func onSurfaceChanged(surfaceId: String, rect: SurfaceRect) {
        // 在onSurfaceChanged中调用ChangeSurface绘制内容
        unsafe {
            cj_LottieController_onSurfaceChanged(ptr, Int64.parse(surfaceId), UInt64(rect.surfaceWidth), UInt64(rect.surfaceHeight))
        }
    }

    //protected override func onSurfaceDestroyed(surfaceId: String) {
    //}

    public func getPtr(): Int64 {
        return this.ptr
    }

    public func getAnimatorPtr(): Int64 {
        return this.animator.ptr
    }


    /**
     * 动画ID，唯一标识
     */
    var lottieId_: String = ""
    public var lottieIdIsUpdate: Bool = false
    public mut prop lottieId: String {
        get() {
            unsafe {
                if (lottieIdIsUpdate) {
                    return this.lottieId_
                }

                let res: CJBytesArray = cj_LottieController_getId(this.ptr)
                let str = BytesArrayToString(res)
                free_CJBytesArray(res)
                lottieIdIsUpdate = true
                return str
            }
        }
        set(value) {
            unsafe {
                if (value.isEmpty()) {
                    return
                }
                lottieIdIsUpdate = false
                let flag: Bool = cj_LottieController_setId(this.ptr, LibC.mallocCString(value))
                if (!flag) {
                    Hilog.error(0, "[LottieController]", "[setlottieId]-Failed to set lottie id to LottieController.")
                } else {
                    this.lottieId_ = value
                    lottieIdIsUpdate = true
                }
            }
        }
    }

    /**
     * 是否成功加载
     */
    public prop isLoaded: Option<Bool> {
        get() {
            unsafe {
                let res: CJBoolStruct = cj_LottieController_isLoaded(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 当前帧号，默认为浮点数，设置非尽可能更新动画帧率时为整数
     */
    public prop currentFrame: Option<Float64> {
        get() {
            unsafe {
                let res: CJFloat64Struct = cj_LottieController_get_currentFrame(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    public func setCurrentFrame(currentFrame: Float64): Unit{
        unsafe {
            cj_LottieController_set_currentFrame(this.ptr, currentFrame)
        }
    }


    /**
     * 当前帧号，浮点数
     */
    public prop currentRawFrame: Option<Float64> {
        get() {
            unsafe {
                let res: CJFloat64Struct = cj_LottieController_get_currentRawFrame(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    public func setCurrentRawFrame(currentRawFrame: Float64): Unit{
        unsafe {
            cj_LottieController_set_currentRawFrame(this.ptr, currentRawFrame)
        }
    }

    /**
     * 动画播放首帧
     */
    public prop firstFrame: Option<Float64> {
        get() {
            unsafe {
                let res: CJFloat64Struct = cj_LottieController_get_firstFrame(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 动画总帧数
     */
    public prop totalFrames: Option<Float64> {
        get() {
            unsafe {
                let res: CJFloat64Struct = cj_LottieController_get_totalFrames(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 动画帧率，帧率 frame/s
     */
    public prop frameRate: Option<Float64> {
        get() {
            unsafe {
                let res: CJFloat64Struct = cj_LottieController_get_frameRate(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 动画帧率，帧率 frame/ms
     */
    public prop frameMult: Option<Float64> {
        get() {
            unsafe {
                let res: CJFloat64Struct = cj_LottieController_get_frameMult(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 动画播放速度
     */
    public prop playSpeed: Option<Float64> {
        get() {
            unsafe {
                let res: CJFloat64Struct = cj_LottieController_get_playSpeed(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 动画播放方向，1 正向播放；-1 反向播放，0 获取播放方向失败。
     */
    public prop playDirection: Option<Int64> {
        get() {
            unsafe {
                let res: CJInt64Struct = cj_LottieController_get_playDirection(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 动画完成播放的次数
     */
    public prop playCount: Option<Int64> {
        get() {
            unsafe {
                let res: CJInt64Struct = cj_LottieController_get_playCount(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 动画播放是否暂停
     */
    public prop isPaused: Option<Bool> {
        get() {
            unsafe {
                let res: CJBoolStruct = cj_LottieController_get_isPaused(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 动画播放是否自动播放
     */
    public prop autoplay: Option<Bool> {
        get() {
            unsafe {
                let res: CJBoolStruct = cj_LottieController_get_autoplay(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 动画播放结束后, 是否循环播放。默认值 -1, 值为 -1 时代表无限循环播放, 值 >=1 时为设置重复播放的次数
     * 定为 Int64 数据类型，< 0 表示无限循环播放；= 0 表示 不循环播放；> 0 为设置重复播放的次数。
     */
    public prop loop: Option<Int64> {
        get() {
            unsafe {
                let res: CJInt64Struct = cj_LottieController_get_loop(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 当前动画片段完成单次播放的帧数
     */
    public prop timeCompleted: Option<Float64> {
        get() {
            unsafe {
                let res: CJFloat64Struct = cj_LottieController_get_timeCompleted(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 当前动画片段序号
     */
    public prop segmentPos: Option<Int64> {
        get() {
            unsafe {
                let res: CJInt64Struct = cj_LottieController_get_segmentPos(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 是否尽可能更新动画帧率
     */
    public prop isSubframeEnabled: Option<Bool> {
        get() {
            unsafe {
                let res: CJBoolStruct =  cj_LottieController_get_isSubframeEnabled(this.ptr)
                if (res.callResult) {
                    return res.res
                }
                return None
            }
        }
    }

    /**
     * 当前动画播放片段
     */
    public prop segments: Array<(Float64, Float64)> {
        get() {
            unsafe {
                let res: CJAnimationSegmentArray = cj_LottieController_get_segments(this.ptr)
                let size: Int64 = res.size
                let buf: CPointer<AnimationSegment> = res.buf

                let segmentsRanges: Array<(Float64, Float64)> = Array<(Float64, Float64)>(size, repeat: (0.0, 0.0))
                for (i in 0..size) {
                    let animationSegment: AnimationSegment = buf.read(i)
                    segmentsRanges[i] = (Float64(animationSegment.initFrame), Float64(animationSegment.endFrame))
                }
                free_CPointer_AnimationSegment(buf)
                return segmentsRanges
            }
        }
    }

    /**
     * 动画播放
     */
    public func play(): Unit {
        unsafe {
            cj_LottieController_play(ptr)
        }
    }

    /**
     * 动画停止
     */
    public func stop(): Unit {
        unsafe {
            cj_LottieController_stop(ptr)
        }
    }

    /**
     * 动画暂停播放切换
     */
    public func togglePause(): Unit {
        unsafe {
            cj_LottieController_togglePause(ptr)
        }
    }

    /**
     * 动画销毁
     */
    public func destroy(): Unit {
        unsafe {
            cj_LottieController_destroy(ptr)
        }
    }

    /**
     * 动画暂停
     */
    public func pause(): Unit {
        unsafe {
            cj_LottieController_pause(ptr)
        }
    }

    /**
     * 控制动画画面停止在某一帧或某个时刻
     * @param value 帧号(值>=0)或时刻(ms)
     * @param isFrame true按帧控制, false按时间控制, 缺省默认false
     */
    public func goToAndStop(value: Float64, isFrame: ?Bool): Unit {
        unsafe {
            if (let Some(v) <- isFrame) {
                cj_LottieController_goToAndStop(ptr, value, v)
            } else {
                cj_LottieController_goToAndStop(ptr, value, false)
            }
        }
    }

    /**
     * 控制动画画面从在某一帧或某个时刻开始播放
     * @param value 帧号(>=0)或时刻(ms)
     * @param isFrame true按帧控制, false按时间控制, 缺省默认false
     */
    public func goToAndPlay(value: Float64, isFrame: ?Bool): Unit {
        unsafe {
            if (let Some(v) <- isFrame) {
                cj_LottieController_goToAndPlay(ptr, value, v)
            } else {
                cj_LottieController_goToAndPlay(ptr, value, false)
            }
        }
    }

    /**
     * 限定动画资源播放时的整体帧范围
     * @param init 起始帧号
     * @param end 结束帧号
     */
    public func setSegment(start: Float64, end: Float64): Unit {
        unsafe {
            cj_LottieController_setSegment(ptr, start, end)
        }
    }

    /**
     * 设置仅播放指定范围的帧动画
     * @param segments 片段或片段数组 若传入的是数组, 且当前loop!=0, 播放结束后, 仅循环播放最后一个片段
     * @param forceFlag 值为true立刻生效, 值为false循环下次播放的时候生效
     */
    public func playSegments(segments: Array<(Float32, Float32)>, forceFlag: ?Bool): Unit {
        unsafe {
            let len: Int64 = segments.size
            let segmentsArray: Array<AnimationSegment> = Array<AnimationSegment>(len, repeat: AnimationSegment(0.0, 0.0))
            for (i in 0..len) {
                segmentsArray[i] = AnimationSegment(segments[i][0], segments[i][1])
            }
            let handle: CPointerHandle<AnimationSegment> = acquireArrayRawData(segmentsArray)
            if (let Some(v) <- forceFlag) {
                cj_LottieController_playSegments(ptr, handle.pointer, len, v, true)
            } else {
                cj_LottieController_playSegments(ptr, handle.pointer, len, false, false)
            }
            releaseArrayRawData(handle)
        }
    }

    /**
     * 重置动画播放片段, 使动画重新从第一帧开始播放完整动画
     * @param forceFlag 值为true立刻生效, 值为false循环下次播放的时候生效
     */
    public func resetSegments(forceFlag: Bool): Unit {
        unsafe {
            cj_LottieController_resetSegments(ptr, forceFlag)
        }
    }

    /**
     * 设置播放速度
     * @param speed 值为浮点类型, speed>0正向播放, speed<0反向播放, speed=0暂停播放, speed=1.0/-1.0正常速度播放
     */
    public func setSpeed(speed: Float64): Unit {
        unsafe {
            cj_LottieController_setSpeed(ptr, speed)
        }
    }

    /**
     * 设置播放方向
     * @param direction 1为正向, -1为反向
     */
    public func setDirection(direction: Int64): Unit {
        unsafe {
            cj_LottieController_setDirection(ptr, direction)
        }
    }

    /**
     * 设置尽可能的更新动画帧率
     * @param useSubFrames true为使用子帧, false为不使用子帧
     * @result 返回 true 代表执行成功，返回 false 代表执行失败。
     */
    public func setSubframe(useSubFrames: Bool): Bool {
        unsafe {
            return cj_LottieController_setSubframe(ptr, useSubFrames)
        }
    }

    /**
     * 获取动画播放时间
     * @param inFrames 为 true 时获取帧数，值为 false 时获取时间(单位ms)，默认为 false。
     * @result 返回的帧数或者时间值，为 None 时代表获取失败。
     */
    public func getDuration(inFrames: ?Bool): Option<Float64> {
        unsafe {
            let flag: Bool = inFrames ?? false
            let res: CJFloat64Struct = cj_LottieController_getDuration(ptr, flag)
            if (res.callResult) {
                return res.res
            }
            return None
        }
    }

    /**
     * 强制触发指定事件
     * @param name 事件名称 'enterFrame' | 'loopComplete' | 'complete' | 'segmentStart' | 'destroy' | 'config_ready' | 'data_ready' | 'DOMLoaded' | 'error' | 'data_failed' | 'loaded_images'
     */
    public func triggerEvent(name: String): Unit {
        unsafe {
            let names: CString = LibC.mallocCString(name)
            cj_LottieController_triggerEvent(ptr, names)
            LibC.free(names)
        }
    }

    /**
     * 添加监听事件
     * @param name 事件名称
     * @param callback 事件回调函数
     */
    public func addEventListener(name: String, callback: CFunc<() -> Unit>): Unit {
        unsafe {
            let names: CString = LibC.mallocCString(name)
            cj_LottieController_addEventListener(ptr, names, callback)
            LibC.free(names)
        }
    }

    /**
     * 移除监听事件
     * @param name 事件名称
     * @param callback 移除后触发的事件回调函数
     */
    public func removeEventListener(name: String, callback: CFunc<() -> Unit>): Unit {
        unsafe {
            let names: CString = LibC.mallocCString(name)
            cj_LottieController_removeEventListener(ptr, names, callback, true)
            LibC.free(names)
        }
    }

    /**
     * 改变动画某个图层的颜色，当前支持st和fl图层
     * @param layer 图层名称
     * @param startColor 起始颜色，若endColor不传，则startColor为要修改的颜色，支持RGB和RGBA格式
     * @param endColor 结束颜色，支持RGB和RGBA格式
     */
    public func changeColor(layer: String, startColor: Array<Float64>, endColor: Array<Float64>): Unit {
        unsafe {
            let startColorLen: Int64 = startColor.size
            let endColorLen: Int64 = endColor.size
            Hilog.error(0, "lipei", "[changeColor测试]---------------------------------------startColor = ${startColor}")
            let startHandle: CPointerHandle<Float64> = acquireArrayRawData(startColor)
            let endHandle: CPointerHandle<Float64> = acquireArrayRawData(endColor)
            let layers: CString = LibC.mallocCString(layer)
            cj_LottieController_changeColor(ptr, layers, startHandle.pointer, startColorLen, endHandle.pointer, endColorLen)

            LibC.free(layers)
            releaseArrayRawData(startHandle)
            releaseArrayRawData(endHandle)
        }
    }

    /**
     * 设置布局模式
     * @param contentMode 布局模式，Fill,Cover,Top,Bottom,Contain，默认为 Contain
     */
    public func setContentMode(contentMode: String): Unit {
        unsafe {
            let cm: CString = LibC.mallocCString(contentMode)
            cj_LottieController_setContentMode(ptr, cm)
            LibC.free(cm)
        }
    }

    /**
     * 设置动画帧率
     * @param frameRate 动画帧率，范围1~120
     */
    public func setFrameRate(frameRate: Float64): Unit {
        unsafe {
            cj_LottieController_setFrameRate(ptr, frameRate)
        }
    }

    /**
     * 设置动画重载
     * @param config 动画配置
     */
    public func reload(config: AnimationConfig): Unit {
        unsafe {
            let configs: LottieDataConfig = config.toLottieDataConfig()
            cj_LottieController_reload(ptr, configs)
        }
    }
}
