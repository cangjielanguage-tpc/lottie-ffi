/*
 * Copyright (C) 2025 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the 'License');
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an 'AS IS' BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LOTTIETURBO_LOTTIE_MANAGER_MAP_H
#define LOTTIETURBO_LOTTIE_MANAGER_MAP_H

#include "controller/lottie_handler.h"
#include "cangjie/lottie_xcomponent_node.h"
#include <cstdint>
#include <unordered_map>

namespace LottieTurbo {

class LottieManagerMap {
private:
    typedef struct {
        int width = 0;
        int height = 0;
        void *window = nullptr;
        std::shared_ptr<LottieHandler> lottieHandler;
    } contentHandle;

    LottieManagerMap();

    LottieManagerMap(const LottieManagerMap &) = delete;

    LottieManagerMap &operator=(const LottieManagerMap &) = delete;

    LottieManagerMap(LottieManagerMap &&) = delete;

    LottieManagerMap &operator=(LottieManagerMap &&) = delete;

    std::unordered_map<std::string, std::unique_ptr<contentHandle>> lottieHandlerMap_;

public:
    static LottieManagerMap &getInstance();

    std::shared_ptr<LottieHandler> GetHandleById(const std::string &id);

    bool Add(const std::string &id);

    void Delete(const std::string &id);

    bool SetHandleById(const std::string &id, std::shared_ptr<LottieHandler> handle);

    bool SetWindowById(const std::string &id, void *winodw);

    bool SetRectById(const std::string &id, int32_t width, int32_t height);

    bool GetRectById(const std::string &id, int32_t &width, int32_t &height);

    void *GetWindowById(const std::string &id);
};

} // namespace LottieTurbo
#endif // LOTTIETURBO_LOTTIE_MANAGER_MAP_H
