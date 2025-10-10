/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

package lottie
import std.collection.ArrayList

public class LottieSource {

    public func play(lottieId!: String = ""): Unit {
        unsafe {
            let str = LibC.mallocCString(lottieId)
            lottiePlay(str)
            LibC.free(str)
        }
    }

    public func pause(lottieId!: String = ""): Unit {
        unsafe {
            let str = LibC.mallocCString(lottieId)
            lottiePause(str)
            LibC.free(str)
        }
    }

    public func stop(lottieId!: String = ""): Unit {
        unsafe {
            let str = LibC.mallocCString(lottieId)
            lottieStop(str)
            LibC.free(str)
        }
    }

    public func setSpeed(speed: Float64,lottieId!: String = ""): Unit {
        unsafe {
            let str = LibC.mallocCString(lottieId)
            lottieSetSpeed(speed,str)
            LibC.free(str)
        }
    }

    //AnimationDirection 先使用Float64代替
    public func setDirection(direction: Float64,lottieId!: String = ""): Unit {
        unsafe {
            let str = LibC.mallocCString(lottieId)
            lottieSetDirection(direction,str)
            LibC.free(str)
        }
    }

    public func togglePause(lottieId!: String = ""): Unit {
        unsafe {
            let str = LibC.mallocCString(lottieId)
            lottieTogglePause(str)
            LibC.free(str)
        }
    }

    public func destroy(lottieId!: String = ""): Unit {
        unsafe {
            let str = LibC.mallocCString(lottieId)
            lottieDestroy(str)
            LibC.free(str)
        }
    }

    public func setFrameRate(frameRate: Float64): Unit {
        unsafe {
            lottieSetFrameRate(frameRate)
        }
    }

    public func clearFileCache(url!: String = ""): Unit {
        unsafe {
            let str = LibC.mallocCString(url)
            lottieClearFileCache(str)
            LibC.free(str)
        }
    }

    public func resizeFileCache(size: Int64, capacity: Int64): Unit {
        unsafe {
            lottieResizeFileCache(size,capacity)
        }
    }

    public func resizeCache(size: Int64, capacity: Int64): Unit {
        unsafe {
            lottieResizeCache(size,capacity)
        }
    }

    public func clearCache(): Unit {
        unsafe {
            lottieClearCache()
        }
    }

    public func removeCache(lottieId: String): Unit {
        unsafe {
            let str = LibC.mallocCString(lottieId)
            lottieRemoveCache(str)
            LibC.free(str)
        }
    }


    public func renderToImage(lottie: String,frameNum: Float64, width: Float64, height: Float64): Array<UInt8> {
        unsafe {
            let str = LibC.mallocCString(lottie)
            let src = renderToImage(str,frameNum,width,height)
            LibC.free(str)
            var results: Array<UInt8> = Array<UInt8>(src.size * 4,repeat: 0)
            if (src.data.isNotNull()) {
                for (i in 0..src.size) {
                    let data = src.data.read(i)

                    results[i*4] = UInt8((data & 0xFF) & 0xFF )
                    results[i*4 + 1] = UInt8((data >> 8) & 0xFF )
                    results[i*4 + 2] = UInt8((data >> 16) & 0xFF )
                    results[i*4 + 3] = UInt8((data >> 24) & 0xFF )

                }
            }
            LibC.free(src.data)
            return results
        }
    }


}

@FastNative
foreign {

    func lottiePlay(lottieId: CString): Unit
    func lottiePause(lottieId: CString): Unit
    func lottieStop(lottieId: CString): Unit
    func lottieSetSpeed(speed: Float64,lottieId: CString): Unit
    func lottieSetDirection(direction: Float64,lottieId: CString): Unit
    func lottieTogglePause(lottieId: CString): Unit
//    func HandleDisposeLottieNode(lottieId: CString): Unit

    func lottieDestroy(lottieId: CString): Unit
    func lottieSetFrameRate(frameRate: Float64): Unit
    func lottieResizeFileCache(size: Int64, capacity: Int64): Unit
    func lottieResizeCache(size: Int64, capacity: Int64): Unit
    func lottieClearFileCache(url: CString): Unit
    func lottieClearCache(): Unit
    func lottieRemoveCache(lottieId: CString): Unit
    func LottieSetVisible(lottieId: CString, visible: Bool): Unit
    func renderToImage(lottie: CString,frameNum: Float64, width: Float64, height: Float64): ArrayBuffer
}

@C // see ffi_define_struct.h
public struct ArrayBuffer {
    public let data: CPointer<UInt32>
    public let size: Int64

     public init(data: CPointer<UInt32>, size: Int64) {
        this.data = data
        this.size = size
    }
}