/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef RLOTTIE510828_LOTTIE_DATA_CONFIG_H
#define RLOTTIE510828_LOTTIE_DATA_CONFIG_H

#include <stdint.h>  // 提供 uint8_t 的定义

#include "dataManager/lottie_config_data.h"

#include "cangjie/lottie_utils.h"

namespace LottieTurbo {

struct LottieDataConfig {

    /**
     * 应用内的唯一id
     * lottieId 用作控制动画的核心参数，默认可不设置，设置时要唯一，建议使用纯字母和数字
     */
    char *lottieId;

    /**
     * 应用内的动画数据文件路径, 仅限json格式
     * path 路径支持本地文件和网络资源
     * @example path: path: "https://xxx.com/xxxx.json"
     */
    char *path;

    /**
     * json格式的动画数据, 仅限json字符串格式
     */
    char *animationData;

    /**
     * 动画播放结束后, 是否循环播放。默认值 -1, 值为 -1 时代表无限循环播放, 值 >=1 时为设置重复播放的次数
     * 定为 Int64 数据类型，< 0 表示无限循环播放；= 0 表示 不循环播放；> 0 为设置重复播放的次数。
     */
    long long loop;

    /**
     * 自动播放设置
     */
    bool autoplay = false;

    /**
     * 初始化动画资源播放时的整体帧范围
     */
    double initialSegment[2];

    /**
     * 动画填充模式 支持填充模式：Fill, Top, Cover, Bottom, Contain  默认模式是：Contain
     */
    char *contentMode = "Contain";

    /**
     * 设置 animator 的刷帧率，范围 1 ~ 120
     */
    uint8_t frameRate = 30;

    /**
     * 当动画不可见时，是否跳过绘制：设为true则跳过绘制，设为false则不跳过绘制(即无条件绘制)，默认值为true
     */
    bool autoSkip = true;

    /**
     * 控制动画操作
     */
    /* LottieController 对应的 c 端 CJLottieController 对象的指针值。*/
    int64_t controller = 0;

    /**
     * 监听事件
     */
    LottieListenerConfig listener;

    /**
     * 设置待替换的图像资源(url/base64)
     */
    ImageAssetDelegate *setImageAssetDelegate;
    long long setImageAssetDelegateSize;
    

    /**
     * 当前设备的屏幕密度
     */
    double density;

    /**
     * 设置是否使用缓存，默认为 true
     */
    bool useCache = true;

    /**
     * cangjie 端利用 @ohos.animator 和 c 交互，c 端 Animator 对象的指针值。
     */
    int64_t animator;

};
};

#endif //RLOTTIE510828_LOTTIE_DATA_CONFIG_H
