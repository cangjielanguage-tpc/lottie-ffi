/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "cj_lottie_view_controller.h"

namespace LottieTurbo {

extern "C" { // cangjie [LottieViewController] ffi

//CJBytesArray cj_PAGViewController_uniqueID()
//{
//    std::string uniqueID = "PAGView" + std::to_string(UniqueID::Next());
//    CJBytesArray result = stringToCJBytesArray(uniqueID);
//    return result;
//}

long long cj_LottieViewController_init(char *lottieId, double radius)
{
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieViewController_init", "--------lottieId = LottieId:%{public}s", lottieId);
    std::string xcomponentId = lottieId;
    auto node = std::make_shared<LottieXcomponentNode>(xcomponentId, radius);
    //CJLottieViewController *cjViewController = new CJLottieViewController(node);
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
        ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieViewController_onSurfaceCreated", "view is null");
        return;
    }

    OHNativeWindow *window = nullptr;
    OH_NativeWindow_CreateNativeWindowFromSurfaceId(surfaceId, &window);
    view->setWindow(window); 

    //TODO:lipei------【测试中】放开 view->get()编译崩溃 ----------------------------------------------------------------
    //std::shared_ptr<LottieXcomponentNode> lxnode = view->get();
    //if (!lxnode) {
    //    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieViewController_onSurfaceCreated", "get the LottieXcomponentNode is None.");
    //    return;
    //}
    //lxnode->OnSurfaceCreated(window, view->getLottieId(), surfaceWidth, surfaceHeight);
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
