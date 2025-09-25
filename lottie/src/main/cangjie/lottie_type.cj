/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

package lottie

@FastNative
foreign {

    func free_CPointer_AnimationSegment(ptr: CPointer<AnimationSegment>): Unit

}


@C
struct CJBytesArray {
    var size: Int64
    var buf: CPointer<UInt8>

    init(size: Int64, buf: CPointer<UInt8>) {
        this.size = size
        this.buf = buf
    }
}

@C
struct CJAnimationSegmentArray {
    var size: Int64
    var buf: CPointer<AnimationSegment>

    init(size: Int64, buf: CPointer<AnimationSegment>) {
        this.size = size
        this.buf = buf
    }
}

@C
struct CJBoolStruct {
    var res: Bool
    var callResult: Bool

    init(res: Bool, callResult: Bool) {
        this.res = res
        this.callResult = callResult
    }
}

@C
struct CJFloat64Struct {
    var res: Float64
    var callResult: Bool

    init(res: Float64, callResult: Bool) {
        this.res = res
        this.callResult = callResult
    }
}

@C
struct CJInt64Struct {
    var res: Int64
    var callResult: Bool

    init(res: Int64, callResult: Bool) {
        this.res = res
        this.callResult = callResult
    }
}
