/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef RLOTTIE510828_LOTTIE_UTILS_H
#define RLOTTIE510828_LOTTIE_UTILS_H

#include <string>
#include "lottie_type.h"

namespace LottieTurbo {
extern "C" {

struct LottieListenerConfig{

    void (*onDrawFrame)();
    void (*onEnterFrame)();
    void (*onLoopComplete)();
    void (*onComplete)();
    void (*onSegmentStart)();
    void (*onDestroy)();
    void (*onConfigReady)();
    void (*onDataReady)();
    void (*onDOMLoaded)();
    void (*onDataFailed)();
    void (*onLoadedImages)();

};

void safe_copy(void *dest, uint32_t dest_size, const void *src, uint32_t src_size);
CJBytesArray stringToCJBytesArray(const std::string &str);


}
}

#endif //RLOTTIE510828_LOTTIE_UTILS_H
