/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "cj_lottie_view_controller.h"

namespace LottieTurbo {

extern "C" { // cangjie [LottieViewController] ffi

long long cj_LottieViewController_init(char *lottieId, double radius)
{
    std::string xcomponentId = lottieId;
    auto node = std::make_shared<LottieXcomponentNode>(xcomponentId, radius);
    auto cjViewController = std::make_shared<CJLottieViewController>(xcomponentId, node);
    return reinterpret_cast<int64_t>(cjViewController.get());
}

void cj_LottieViewController_onSurfaceCreated(const int64_t jView, const int64_t surfaceId, uint64_t surfaceWidth, uint64_t surfaceHeight)
{
    if (jView == 0) {
        return;
    }
    intptr_t viewPtrInt = static_cast<intptr_t>(jView);
    CJLottieViewController *view = reinterpret_cast<CJLottieViewController *>(viewPtrInt);
    if (!view) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieViewController_onSurfaceCreated", "view is null");
        return;
    }

    OHNativeWindow *window = nullptr;
    OH_NativeWindow_CreateNativeWindowFromSurfaceId(surfaceId, &window);
    view->setWindow(window); 
}

void cj_LottieViewController_onSurfaceChanged(const int64_t jView, const int64_t surfaceId, uint64_t surfaceWidth, uint64_t surfaceHeight)
{
    if (jView == 0) {
        return;
    }
    intptr_t viewPtrInt = static_cast<intptr_t>(jView);
    CJLottieViewController *view = reinterpret_cast<CJLottieViewController *>(viewPtrInt);
    if (!view) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieViewController_onSurfaceChanged", "view is null");
        return;
    }

    OHNativeWindow *window = view->getWindow();
    view->get()->OnSurfaceChanged(window, view->getLottieId(), surfaceWidth, surfaceHeight);
}

};

};
