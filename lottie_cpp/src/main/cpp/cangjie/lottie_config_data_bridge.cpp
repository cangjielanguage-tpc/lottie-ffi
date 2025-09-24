/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "lottie_config_data_bridge.h"
#include "common/common.h"
#include "dataManager/lottie_config_data.h"
#include "controller/lottie_handler.h"
#include "common/lottile_utils.h"

namespace LottieTurbo {

static const size_t SEGMENT_SIZE = 2;
static const size_t IMGASSET_SIZE = 2;


LottieConfigBridge::LottieConfigBridge(LottieDataConfig config)
{
    Parse(config);

    //napi_value workName;
    //napi_create_string_utf8(env, "LottieListener", NAPI_AUTO_LENGTH, &workName);
    //napi_create_threadsafe_function(env, nullptr, nullptr, workName, 0, 1, nullptr, nullptr, nullptr,
    //                                [](napi_env env, napi_value cb, void *context, void *data) {
    //    auto trigger = (LottieHandler::TriggerData *)data;
    //    if (trigger != nullptr) {
    //        trigger->func();
    //        delete trigger;
    //        trigger = nullptr;
    //    }
    //}, &safeFunc_);
    if (listener_ != nullptr) {
        listener_->TriggerEvent(LottieEventType::ON_CONFIG_READY);
    }
}


bool LottieConfigBridge::Parse(LottieDataConfig config)
{
    //处理 loop 字段
    /**
     * 动画播放结束后, 是否循环播放。默认值 -1, 值为 -1 时代表无限循环播放, 值 >=1 时为设置重复播放的次数
     * 定为 Int64 数据类型，< 0 表示无限循环播放；= 0 表示 不循环播放；> 0 为设置重复播放的次数。
     */
    long long loop;
    if (config.loop < 0) {
        this->loop_ = true;
    } else if (config.loop == 0) {
        this->loop_ = false;
    } else {//(config.loop > 0
        this->loop_ = (int)config.loop;
    }

    //处理 autoplay 字段
    this->autoplay_ = config.autoplay;

    //处理 animationData/path 字段
    //TODO:暂时不做 path 字段兼容 Resource 类型。
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieConfigBridge::Parse】----------------------this->animationData_.length() = %{public}d", this->animationData_.length());
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieConfigBridge::Parse】----------------------strlen(config.animationData) = %{public}d", strlen(config.animationData));
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieConfigBridge::Parse】----------------------config.animationData != 空字符串 = %{public}d", config.animationData != "");
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieConfigBridge::Parse】----------------------strcmp(config.animationData, 空字符串) == 0 = %{public}d", strcmp(config.animationData, "") == 0);
    ////OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieConfigBridge::Parse】----------------------config.animationData = %{public}s", config.animationData);

    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieConfigBridge::Parse】----------------------config.path != 空字符串 = %{public}d", config.path != "");
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieConfigBridge::Parse】----------------------strcmp(config.path, 空字符串) == 0 = %{public}d", strcmp(config.path, "") == 0);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieConfigBridge::Parse】----------------------config.path = %{public}s", config.path);
    if (strcmp(config.animationData, "") != 0) {
        this->animationData_ = config.animationData;
    } else if (strcmp(config.path, "") != 0) {
        this->path_ = config.path;
    }

    //处理 initialSegment 字段
    if (config.initialSegment[0] != config.initialSegment[1]) {
        this->initialSegment_.initFrame = config.initialSegment[0];
        this->initialSegment_.endFrame = config.initialSegment[1];
    }

    //处理 lottieId 字段
    if (config.lottieId != "") {
        this->lottieId_ = config.lottieId;
    }

    //处理 contentMode 字段
    if (config.contentMode != "") {
        this->contentMode_ = config.contentMode;
    }

    //处理 frameRate 字段--设置 animator 的刷帧率，范围 1 ~ 120
    if (config.frameRate >= 1 && config.frameRate <= 120) {
        this->frameRate_ = config.frameRate;
    }

    //处理 setImageAssetDelegate 字段
    if (config.setImageAssetDelegateSize > 0) {
        //std::make_shared<std::vector<ImageAssetDelegate>> imgAssets = std::make_shared<std::vector<ImageAssetDelegate>>();
        std::vector<ImageAssetDelegate> imgAssets = std::vector<ImageAssetDelegate>();
        for (int index = 0; index < config.setImageAssetDelegateSize; index++) {
            auto rst = config.setImageAssetDelegate[index];
            ImageAssetDelegate iad = ImageAssetDelegate();
            iad.refId = rst.refId;
            iad.fileName = rst.fileName;
            
            //imgAssets.push_back(iad);
            imgAssets.emplace_back(iad);  // 更高效的方式
        }
        this->imgAssets_ = std::make_shared<std::vector<ImageAssetDelegate>>(imgAssets);
    }

    //处理 useCache 字段
    this->useCache_ = config.useCache;

    //处理 density 字段
    DENSITY_DPI = config.density;

    //TODO: 处理 resManager 字段 -- 
    //TODO: cj 的 ResourceManager 对象传不到 c代码层，改成在 cj 直接处理资源，转为 json 字符串或者 Byte 数组？？？
    //if (!GetResourceManager(value)) {
    //    DRAWING_LOGD("Parse context error");
    //}
    
    //处理 listener 字段
    if (listener_ == nullptr) {
        listener_ = std::make_unique<LottieListener>();
    }
    listener_->InitListenerCallback(config.listener);
    

    /**
     * cangjie 端利用 @ohos.animator 和 c 交互，c 端 Animator 对象的指针值。
     */
    this->animator = config.animator;
    
    return true;
}

std::unique_ptr<LottieListener> &LottieConfigBridge::GetLottieListener()
{
    return listener_;
}

//napi_threadsafe_function LottieConfigBridge::GetSafeFunction()
//{
//    return safeFunc_;
//}

}
