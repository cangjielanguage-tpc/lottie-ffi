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


//TODO:lipei-----------------------------------------------
//LottieListener::LottieListener(napi_env env) : napiHelper_(NapiHelper(env))
LottieListener::LottieListener()
{
    DRAWING_LOGD("LottieListener构造");
}

LottieListener::~LottieListener()
{
    DRAWING_LOGD("LottieListener析构");
    funcMap_.clear();
    //TODO:lipei-----------------------------------------------
    //for (auto &pair : callbackRefs_) {
    //    this->DeleteCallbackReference(pair.second);
    //}
    //callbackRefs_.clear();
}

void LottieListener::InitListenerCallback(LottieListenerConfig listenerCJObject)
{
    // 确保 jsObject 是个对象
    //if (!napiHelper_.CheckIsType(listenerCJObject, napi_object)) {
    //    return;
    //}
    //jsThisRef_ = napiHelper_.CreateReference(listenerCJObject);

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

    //const std::string &funcName = it->second;
    //if (!napiHelper_.CheckFuncIsExist(listenerCJObject, funcName)) {
    //    return;
    //}

    if (it->first == LottieEventType::ON_DRAW_FRAME) {// && listenerCJObject.hasOnDrawFrame) {
        this->RegisterCallback(listenerCJObject.onDrawFrame, eventType);
    } else if (it->first == LottieEventType::ON_ENTER_FRAME) {// && listenerCJObject.hasOnEnterFrame) {
        this->RegisterCallback(listenerCJObject.onEnterFrame, eventType);
    } else if (it->first == LottieEventType::ON_LOOP_COMPLETE) {// && listenerCJObject.hasOnLoopComplete) {
        this->RegisterCallback(listenerCJObject.onLoopComplete, eventType);
    } else if (it->first == LottieEventType::ON_COMPLETE) {// && listenerCJObject.hasOnComplete) {
        this->RegisterCallback(listenerCJObject.onComplete, eventType);
    } else if (it->first == LottieEventType::ON_SEGMENT_START) {// && listenerCJObject.hasOnSegmentStart) {
        this->RegisterCallback(listenerCJObject.onSegmentStart, eventType);
    } else if (it->first == LottieEventType::ON_DESTROY) {// && listenerCJObject.hasOnDestroy) {
        this->RegisterCallback(listenerCJObject.onDestroy, eventType);
    } else if (it->first == LottieEventType::ON_CONFIG_READY) {// && listenerCJObject.hasOnConfigReady) {
        this->RegisterCallback(listenerCJObject.onConfigReady, eventType);
    } else if (it->first == LottieEventType::ON_DATA_READY) {// && listenerCJObject.hasOnDataReady) {
        this->RegisterCallback(listenerCJObject.onDataReady, eventType);
    } else if (it->first == LottieEventType::ON_DOM_LOADED) {// && listenerCJObject.hasOnDOMLoaded) {
        this->RegisterCallback(listenerCJObject.onDOMLoaded, eventType);
    } else if (it->first == LottieEventType::ON_DATA_FAILED) {// && listenerCJObject.hasOnDataFailed) {
        this->RegisterCallback(listenerCJObject.onDataFailed, eventType);
    } else if (it->first == LottieEventType::ON_LOADED_IMAGES) {// && listenerCJObject.hasOnLoadedImages) {
        this->RegisterCallback(listenerCJObject.onLoadedImages, eventType);
    }
    //auto jsFunc = napiHelper_.GetObjectProperty(listenerCJObject, funcName);
    //this->RegisterCallback(jsFunc, eventType);
}

void LottieListener::RegisterCallback(std::function<void()> callbackFunc, LottieEventType eventType)
{
    auto it = lottieEventMap.find(eventType);
    if (it == lottieEventMap.end()) {
        // 非法事件类型
        return;
    }
    // 先删除旧的回调引用，避免内存泄漏
    //if (callbackRefs_.find(eventType) != callbackRefs_.end()) {
    //    napiHelper_.DeleteReference(callbackRefs_[eventType]);
    //}
    // 保存回调引用，延长生命周期
    //callbackRefs_[eventType] = napiHelper_.CreateReference(callbackFunc);

    // 注册回调
    //funcMap_[eventType] = [this, eventType]() {
    //    // 判断回调引用是否存在
    //    auto it = callbackRefs_.find(eventType);
    //    if (it == callbackRefs_.end()) {
    //        return;
    //    }
    //    auto jsFunc = napiHelper_.GetReferenceValue(it->second);
    //    auto jsThis = napiHelper_.GetReferenceValue(jsThisRef_);
    //    napiHelper_.Call(jsThis, jsFunc, 0, nullptr);
    //};
    
    funcMap_[eventType] = callbackFunc;
}


void LottieListener::UnRegisterCallback(LottieEventType eventName)
{
    auto it = funcMap_.find(eventName);
    if (it != funcMap_.end()) {
        funcMap_.erase(it);

        //TODO:lipei-----------------------------------------------
        //auto refIt = callbackRefs_.find(eventName);
        //if (refIt != callbackRefs_.end()) {
        //    napiHelper_.DeleteReference(refIt->second);
        //    callbackRefs_.erase(refIt);
        //}
    }
}

void LottieListener::TriggerEvent(LottieEventType eventType)
{
    auto it = funcMap_.find(eventType);
    if (it != funcMap_.end()) {
        it->second();
    }
}

/*
void LottieListener::DeleteCallbackReference(napi_ref &callbackRef)
{
    if (callbackRef != nullptr) {
        napiHelper_.DeleteReference(callbackRef);
        callbackRef = nullptr;
    }
}
*/

} // namespace LottieTurbo
