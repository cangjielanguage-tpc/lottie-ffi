/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#ifndef RLOTTIE510828_LOTTIE_CONFIG_DATA_BRIDGE_H
#define RLOTTIE510828_LOTTIE_CONFIG_DATA_BRIDGE_H

#include "dataManager/lottie_config_data.h"
#include "cangjie/lottie_listener.h"
//#include "common/napi_helper.h"
#include "dataManager/data_parse.h"
#include <rawfile/raw_file_manager.h>
//#include <napi/native_api.h>
#include <string>

#include "cangjie/lottie_data_config.h"

#include "controller/lottie_handler.h"

namespace LottieTurbo {

class LottieConfigBridge : public LottieConfig {
public:
    explicit LottieConfigBridge(LottieDataConfig config);
    //template <typename T>
    //bool GetConfigValue(napi_value &jsObject, T &value, std::string const &parameter);
    //bool GetResource(napi_value &jsObject, Resource &resource, std::string parameter);
    //bool GetConfigLoopValue(napi_value &jsObject, std::variant<bool, int> &value,
    //                                    std::string const &parameter);
    //bool GetResourceManager(napi_value &jsObject);
    //template <typename T> T *GetObject(napi_value &jsObject);
    std::unique_ptr<LottieListener> &GetLottieListener();

    std::function<void(void *)> GetSafeFunction();
    void SetSafeFunction(std::function<void(void *)> safeFunc);

private:
    //NapiHelper napiHelper_;
    bool Parse(LottieDataConfig config);
    //bool ConvertToType(napi_value &napiValue, bool &boolValue);
    //bool ConvertToType(napi_value &napiValue, std::string &stringValue);
    //bool ConvertToType(napi_value &napiValue, int &intValue);
    //bool ConvertToType(napi_value &napiValue, double &doubleValue);
    //bool ConvertToType(napi_value &napiValue, AnimationSegment &animationSegment);
    //bool ConvertToType(napi_value &napiValue, napi_value &jsObject);
    //bool ConvertToType(napi_value &napiValue, Resource &resource);
    //bool ConvertToType(napi_value &napiValue, ImageAssetDelegateType &imgAssets);
    std::unique_ptr<LottieListener> listener_{nullptr};

    std::function<void(void *)> safeFunc_{nullptr};
};

//template <typename T>
//bool LottieConfigBridge::GetConfigValue(napi_value &jsObject, T &value, std::string const &parameter)
//{
//    bool isExist = napiHelper_.CheckPropIsExist(jsObject, parameter);
//    if (!isExist) {
//        return false;
//    }
//    auto napiValue = napiHelper_.GetObjectProperty(jsObject, parameter);
//    return ConvertToType(napiValue, value);
//}
//
//template <typename T> T *LottieConfigBridge::GetObject(napi_value &jsObject)
//{
//    return napiHelper_.UnwrapInstance<T>(jsObject);
//}

} // namespace LottieTurbo

#endif //RLOTTIE510828_LOTTIE_CONFIG_DATA_BRIDGE_H
