/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef RLOTTIE510828_CJ_LOTTIE_VIEW_CONTROLLER_H
#define RLOTTIE510828_CJ_LOTTIE_VIEW_CONTROLLER_H

#include "cangjie/lottie_xcomponent_node.h"
//#include "cangjie/cj_unique_id.h"

#include <native_window/external_window.h>

namespace LottieTurbo {

class CJLottieViewController {
public:
    explicit CJLottieViewController(const std::string &xcomponentId, std::shared_ptr<LottieXcomponentNode> &lottieXcomponentNode) : xcomponentId(std::move(xcomponentId)), lottieXcomponentNode(lottieXcomponentNode) {}

    ~CJLottieViewController(){};

    static std::string ClassName() { return "CJLottieViewController"; }

    std::shared_ptr<LottieXcomponentNode> get() { return lottieXcomponentNode; }
    
    std::string getLottieId() { return xcomponentId; }
    
    OHNativeWindow* getWindow() { return window; }
    void setWindow(OHNativeWindow *window) { this->window = window; }

private:
    //TODO：不能直接用 OH_NativeXComponent，需要用自定义 XComponent。
    //void OnSurfaceCreated(OH_NativeXComponent *component, void *window);
    //void OnSurfaceChanged(OH_NativeXComponent *component, void *window);
    
    std::shared_ptr<LottieXcomponentNode> lottieXcomponentNode;
    //std::shared_ptr<LottieXcomponentNode> &lottieXcomponentNode;
    OHNativeWindow *window = nullptr;
    
    std::string xcomponentId;
};

extern "C" { // cangjie [LottieViewController] ffi

//CJBytesArray cj_PAGViewController_uniqueID();

long long cj_LottieViewController_init(char *lottieId, double radius);
void cj_LottieViewController_onSurfaceCreated(const int64_t jView, const int64_t surfaceId, uint64_t surfaceWidth, uint64_t surfaceHeight);
void cj_LottieViewController_onSurfaceChanged(const int64_t jView, const int64_t surfaceId, uint64_t surfaceWidth, uint64_t surfaceHeight);

};

};

#endif //RLOTTIE510828_CJ_LOTTIE_VIEW_CONTROLLER_H
