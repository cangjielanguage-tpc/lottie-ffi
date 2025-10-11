/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

package lottie

internal import ohos.hilog.*

@FastNative
foreign func free_CJBytesArray(ptr: CJBytesArray): Unit

func BytesArrayToString(res: CJBytesArray): String {
    unsafe {
        let size: Int64 = res.size
        if (size == 0) {
            return "";
        }
        let buf: CPointer<UInt8> = res.buf
        let bytes: Array<UInt8> = Array<UInt8>(size, repeat: 0)
        for (i in 0..size) {
            if (buf.isNotNull()) {
                bytes[i] = buf.read(i)
            }
        }
        let str = String.fromUtf8(bytes)
        return str
    }
}
