/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef LOTTIETURBO_LOTTIELISTENER_H
#define LOTTIETURBO_LOTTIELISTENER_H

//TODO:lipei-----------------------------------------------
//#include "common/napi_helper.h"
//#include "napi/native_api.h"
#include <functional>

#include <unordered_map>

#include "cangjie/lottie_utils.h"

namespace LottieTurbo {

enum class LottieEventType {
    ON_DRAW_FRAME,
    ON_ENTER_FRAME,
    ON_LOOP_COMPLETE,
    ON_COMPLETE,
    ON_SEGMENT_START,
    ON_DESTROY,
    ON_CONFIG_READY,
    ON_DATA_READY,
    ON_DOM_LOADED,
    ON_ERROR,
    ON_DATA_FAILED,
    ON_LOADED_IMAGES
};

class LottieListener {
public:
    //TODO:lipei-----------------------------------------------
    //explicit LottieListener(napi_env env);
    explicit LottieListener();
    ~LottieListener();

    /**
     * @brief 注册LottieListener对象所有存在的事件回调
     * @param listenerJsObject JS侧的LottieListener对象
     */
    //TODO:lipei-----------------------------------------------
    //void InitListenerCallback(napi_value listenerJsObject);
    void InitListenerCallback(LottieListenerConfig listenerCJObject);

    /**
     * @brief 注册制定的事件回调
     * @param callbackFunc 回调函数
     * @param eventType 需要注册回调的事件类型
     */
    //TODO:lipei-----------------------------------------------
    //void RegisterCallback(napi_value callbackFunc, LottieEventType eventType);
    void RegisterCallback(std::function<void()> callbackFunc, LottieEventType eventType);

    /**
     * @brief 取消注册指定的事件回调
     * @param eventName LottieEventType
     */
    void UnRegisterCallback(LottieEventType eventType);

    /**
     * @brief 触发事件回调
     * @param eventName
     */
    void TriggerEvent(LottieEventType eventType);

private:
    /**
     * @brief 注册LottieListener自身的某个事件回调
     * @param listenerJsObject JS侧的LottieListener对象
     * @param eventType 需要注册回调的事件类型
     */
    //TODO:lipei-----------------------------------------------
    //void RegisterSelfCallback(napi_value listenerJsObject, LottieEventType eventType);
    void RegisterSelfCallback(LottieListenerConfig listenerCJObject, LottieEventType eventType);

    /**
     * @brief 删除回调引用
     * @param callbackRef napi_ref
     */
    //TODO:lipei-----------------------------------------------
    //void DeleteCallbackReference(napi_ref &callbackRef);

    //TODO:lipei-----------------------------------------------
    //NapiHelper napiHelper_;
    //napi_ref jsThisRef_;
    std::unordered_map<LottieEventType, std::function<void()>> funcMap_; // 事件与回调函数的映射
    //TODO:lipei-----------------------------------------------
    //std::unordered_map<LottieEventType, napi_ref> callbackRefs_;         // 事件与回调引用的映射
};

} // namespace LottieTurbo
#endif // LOTTIETURBO_LOTTIELISTENER_H
