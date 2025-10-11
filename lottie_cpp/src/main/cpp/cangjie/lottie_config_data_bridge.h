/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef RLOTTIE510828_LOTTIE_CONFIG_DATA_BRIDGE_H
#define RLOTTIE510828_LOTTIE_CONFIG_DATA_BRIDGE_H

#include "dataManager/lottie_config_data.h"
#include "cangjie/lottie_listener.h"
#include "dataManager/data_parse.h"
#include <rawfile/raw_file_manager.h>
#include <string>

#include "cangjie/lottie_data_config.h"
#include "controller/lottie_handler.h"

namespace LottieTurbo {

class LottieConfigBridge : public LottieConfig {
public:
    explicit LottieConfigBridge(LottieDataConfig config);

    std::unique_ptr<LottieListener> &GetLottieListener();

    std::function<void(void *)> GetSafeFunction();
    void SetSafeFunction(std::function<void(void *)> safeFunc);

private:
    bool Parse(LottieDataConfig config);

    std::unique_ptr<LottieListener> listener_{nullptr};

    std::function<void(void *)> safeFunc_{nullptr};
};

} // namespace LottieTurbo

#endif //RLOTTIE510828_LOTTIE_CONFIG_DATA_BRIDGE_H
