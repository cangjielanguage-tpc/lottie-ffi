/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "cangjie/lottie_listener.h"
#include "common/common.h"

namespace LottieTurbo {

static const std::unordered_map<LottieEventType, std::string> lottieEventMap = {
    {LottieEventType::ON_DRAW_FRAME, "onDrawFrame"},       {LottieEventType::ON_ENTER_FRAME, "onEnterFrame"},
    {LottieEventType::ON_LOOP_COMPLETE, "onLoopComplete"}, {LottieEventType::ON_COMPLETE, "onComplete"},
    {LottieEventType::ON_SEGMENT_START, "onSegmentStart"}, {LottieEventType::ON_DESTROY, "onDestroy"},
    {LottieEventType::ON_CONFIG_READY, "onConfigReady"},   {LottieEventType::ON_DATA_READY, "onDataReady"},
    {LottieEventType::ON_DOM_LOADED, "onDOMLoaded"},       {LottieEventType::ON_ERROR, "onError"},
    {LottieEventType::ON_DATA_FAILED, "onDataFailed"},     {LottieEventType::ON_LOADED_IMAGES, "onLoadedImages"}};



LottieListener::LottieListener()
{
    DRAWING_LOGD("LottieListener构造");
}

LottieListener::~LottieListener()
{
    DRAWING_LOGD("LottieListener析构");
    funcMap_.clear();
}

void LottieListener::InitListenerCallback(LottieListenerConfig listenerCJObject)
{
    for (auto &pair : lottieEventMap) {
        this->RegisterSelfCallback(listenerCJObject, pair.first);
    }
}

void LottieListener::RegisterSelfCallback(LottieListenerConfig listenerCJObject, LottieEventType eventType)
{
    auto it = lottieEventMap.find(eventType);
    if (it == lottieEventMap.end()) {
        // 非法事件类型
        return;
    }

    if (it->first == LottieEventType::ON_DRAW_FRAME) {
        this->RegisterCallback(listenerCJObject.onDrawFrame, eventType);
    } else if (it->first == LottieEventType::ON_ENTER_FRAME) {
        this->RegisterCallback(listenerCJObject.onEnterFrame, eventType);
    } else if (it->first == LottieEventType::ON_LOOP_COMPLETE) {
        this->RegisterCallback(listenerCJObject.onLoopComplete, eventType);
    } else if (it->first == LottieEventType::ON_COMPLETE) {
        this->RegisterCallback(listenerCJObject.onComplete, eventType);
    } else if (it->first == LottieEventType::ON_SEGMENT_START) {
        this->RegisterCallback(listenerCJObject.onSegmentStart, eventType);
    } else if (it->first == LottieEventType::ON_DESTROY) {
        this->RegisterCallback(listenerCJObject.onDestroy, eventType);
    } else if (it->first == LottieEventType::ON_CONFIG_READY) {
        this->RegisterCallback(listenerCJObject.onConfigReady, eventType);
    } else if (it->first == LottieEventType::ON_DATA_READY) {
        this->RegisterCallback(listenerCJObject.onDataReady, eventType);
    } else if (it->first == LottieEventType::ON_DOM_LOADED) {
        this->RegisterCallback(listenerCJObject.onDOMLoaded, eventType);
    } else if (it->first == LottieEventType::ON_DATA_FAILED) {
        this->RegisterCallback(listenerCJObject.onDataFailed, eventType);
    } else if (it->first == LottieEventType::ON_LOADED_IMAGES) {
        this->RegisterCallback(listenerCJObject.onLoadedImages, eventType);
    }

}

void LottieListener::RegisterCallback(std::function<void()> callbackFunc, LottieEventType eventType)
{
    auto it = lottieEventMap.find(eventType);
    if (it == lottieEventMap.end()) {
        // 非法事件类型
        return;
    }
    // 先删除旧的回调引用，避免内存泄漏
    // 保存回调引用，延长生命周期
    // 注册回调

    funcMap_[eventType] = callbackFunc;
}


void LottieListener::UnRegisterCallback(LottieEventType eventName)
{
    auto it = funcMap_.find(eventName);
    if (it != funcMap_.end()) {
        funcMap_.erase(it);
    }
}

void LottieListener::TriggerEvent(LottieEventType eventType)
{
    auto it = funcMap_.find(eventType);
    if (it != funcMap_.end()) {
        it->second();
    }
}

} // namespace LottieTurbo
