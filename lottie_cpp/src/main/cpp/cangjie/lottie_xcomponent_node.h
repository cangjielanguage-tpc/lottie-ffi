/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef RLOTTIE510828_LOTTIE_XCOMPONENT_NODE_H
#define RLOTTIE510828_LOTTIE_XCOMPONENT_NODE_H

#include "controller/lottie_handler.h"
#include "render/lottie_render.h"
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <cstdint>
#include <string>

#include "hilog/log.h"

namespace LottieTurbo {

class LottieXcomponentNode {
public:
    struct CalculatePosition {
        bool keepAspectRatio{true};
        int width{0};
        int height{0};
        int x{0};
        int y{0};
    };

public:
    LottieXcomponentNode(std::string xcomponentId, float radius);
    ~LottieXcomponentNode();

    /**
     * @brief 创建Xcomponent组件并节点上树
     * @param xcomponentId 组件Id
     */
    void CreateXcomponentNode(std::string xcomponentId, float radius);

    /**
     * 初始化回调函数并渲染第一帧
     */
    void Init();

    /**
     * 调度渲染任务到线程池
     * @param absoluteFrameToPlay 要渲染的绝对帧
     * @param current 当前索引
     */
    void ScheduleRenderTask(float absoluteFrameToPlay, int64_t current);

    /**
     * 渲染当前帧（仅绘制）
     */
    void RenderCurrentFrame();

    /**
     * 渲染当前帧并推进到下一个待绘制帧
     */
    void RenderAndAdvanceFrame();

    /**
     * 更新布局模式
     */
    void OnUpdateContentMode();

    /**
     * 设置定时器
     */
    void SetAnimator();

    /**
     * 绑定LottieHandler
     */
    void SetHandler(std::shared_ptr<LottieHandler> LottieHandler);

    void OnSurfaceCreated(void *window, std::string lottieId, uint64_t surfaceWidth, uint64_t surfaceHeight);
    void OnSurfaceChanged(void *window, std::string lottieId, uint64_t surfaceWidth, uint64_t surfaceHeight);

    void destroy(uint64_t surfaceWidth, uint64_t surfaceHeight);
   
private:

    /**
     * 渲染首帧和重置lottie宽高刷新渲染:
     * OnSurfaceCreated回调时渲染首帧
     * OnSurfaceChanged回调时重置lottie宽高刷新渲染
     * @param component XComponent回调的OH_NativeXComponent指针
     * @param window XComponent回调的window指针
     * @param isOnSurfaceCreated 用于区分OnSurfaceCreated和OnSurfaceChanged的回调
     * @return void
     */
    //不能直接用 OH_NativeXComponent，需要用自定义 XComponent。
    static void RenderOnSurfaceChanged(void *window, std::string lottieId, uint64_t surfaceWidth, uint64_t surfaceHeight, bool isOnSurfaceCreated);

    std::shared_ptr<LottieHandler> lottieHandler_{nullptr};
    std::shared_ptr<LottieRender> lottieRender_{nullptr};
    std::string contentMode_;
    CalculatePosition position_;
    std::atomic<int8_t> status_{0};
    bool firstFrame_{true};
};

extern "C" { // cangjie [LottieXcomponentNode] ffi

//int64_t cj_LottieViewController_createLottieView();



};

};
#endif //RLOTTIE510828_LOTTIE_XCOMPONENT_NODE_H
