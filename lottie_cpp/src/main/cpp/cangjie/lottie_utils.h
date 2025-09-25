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
//    bool hasOnDrawFrame = false;
//    bool hasOnEnterFrame = false;
//    bool hasOnLoopComplete = false;
//    bool hasOnComplete = false;
//    bool hasOnSegmentStart = false;
//    bool hasOnDestroy = false;
//    bool hasOnConfigReady = false;
//    bool hasOnDataReady = false;
//    bool hasOnDOMLoaded = false;
//    bool hasOnDataFailed = false;
//    bool hasOnLoadedImages = false;

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
//    std::function<void()> onDrawFrame;
//    std::function<void()> onEnterFrame;
//    std::function<void()> onLoopComplete;
//    std::function<void()> onComplete;
//    std::function<void()> onSegmentStart;
//    std::function<void()> onDestroy;
//    std::function<void()> onConfigReady;
//    std::function<void()> onDataReady;
//    std::function<void()> onDOMLoaded;
//    std::function<void()> onDataFailed;
//    std::function<void()> onLoadedImages;

};

void safe_copy(void *dest, uint32_t dest_size, const void *src, uint32_t src_size);
CJBytesArray stringToCJBytesArray(const std::string &str);


}
}

#endif //RLOTTIE510828_LOTTIE_UTILS_H
