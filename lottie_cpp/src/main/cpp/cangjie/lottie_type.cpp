/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "lottie_type.h"

namespace LottieTurbo {

extern "C" {

void free_CPointer_AnimationSegment(AnimationSegment *ptr)
{
    if (ptr != nullptr) {
        /*
         * There are no member variables of CJPAGVideoRange in the array that need to be released. Directly release the
         * array memory here.
         */
        delete[] ptr;
        ptr = nullptr;
    }
}

void free_CJBytesArray(CJBytesArray ptr)
{
    if (ptr.size == 0) {
        return;
    }
    uint8_t *buf = ptr.buf;
    if (buf != nullptr) {
        free(buf);
    }
}


}

}
