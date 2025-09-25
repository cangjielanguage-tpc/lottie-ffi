/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef RLOTTIE510828_LOTTIE_TYPE_H
#define RLOTTIE510828_LOTTIE_TYPE_H

#include <stdint.h>
#include "dataManager/lottie_config_data.h"

namespace LottieTurbo {

extern "C" {

struct CJBytesArray {
    int64_t size;
    uint8_t *buf;
};

struct CJAnimationSegmentArray {
    int64_t size;
    AnimationSegment *buf;
};

struct CJBoolStruct {
    bool res;
    bool callResult = false;
};

struct CJFloat64Struct {
    double res;
    bool callResult = false;
};

struct CJInt64Struct {
    long long res;
    bool callResult = false;
};


void free_CPointer_AnimationSegment(AnimationSegment *ptr);
void free_CJBytesArray(CJBytesArray ptr);

}

}

#endif //RLOTTIE510828_LOTTIE_TYPE_H
