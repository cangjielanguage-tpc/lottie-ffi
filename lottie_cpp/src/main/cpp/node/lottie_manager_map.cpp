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
#include "node/lottie_manager_map.h"

namespace LottieTurbo {

LottieManagerMap::LottieManagerMap()
{
}

LottieManagerMap &LottieManagerMap::getInstance()
{
    static LottieManagerMap instance;
    return instance;
}

std::shared_ptr<LottieHandler> LottieManagerMap::GetHandleById(const std::string &id)
{
    auto it = lottieHandlerMap_.find(id);
    if (it != lottieHandlerMap_.end()) {
        if (it->second->lottieHandler) {
            return it->second->lottieHandler;
        }
    }
    return std::shared_ptr<LottieHandler>(nullptr);
}

bool LottieManagerMap::SetHandleById(const std::string &id, std::shared_ptr<LottieHandler> handle)
{
    auto it = lottieHandlerMap_.find(id);
    if (it != lottieHandlerMap_.end()) {
        it->second->lottieHandler = handle;
        return true;
    }
    return false;
}

bool LottieManagerMap::SetRectById(const std::string &id, int32_t width, int32_t height)
{
    auto it = lottieHandlerMap_.find(id);
    if (it != lottieHandlerMap_.end()) {
        it->second->width = width;
        it->second->height = height;
        return true;
    }
    return false;
}

bool LottieManagerMap::GetRectById(const std::string &id, int32_t &width, int32_t &height)
{
    auto it = lottieHandlerMap_.find(id);
    if (it != lottieHandlerMap_.end()) {
        width = it->second->width;
        height = it->second->height;
        return true;
    }
    return false;
}

bool LottieManagerMap::SetWindowById(const std::string &id, void *window)
{
    auto it = lottieHandlerMap_.find(id);
    if (it != lottieHandlerMap_.end()) {
        it->second->window = window;
        return true;
    }
    return false;
}

void *LottieManagerMap::GetWindowById(const std::string &id)
{
    auto it = lottieHandlerMap_.find(id);
    if (it != lottieHandlerMap_.end()) {
        if (it->second->window) {
            return it->second->window;
        }
    }
    return nullptr;
}

bool LottieManagerMap::Add(const std::string &id)
{
    if (id.empty()) {
        return false;
    }
    auto mem = std::make_unique<contentHandle>();
    mem->width = 0;
    mem->height = 0;
    mem->window = nullptr;
    mem->lottieHandler = nullptr;
    lottieHandlerMap_.insert({id, std::move(mem)});
    return true;
}

void LottieManagerMap::Delete(const std::string &id)
{
    auto it = lottieHandlerMap_.find(id);
    if (it == lottieHandlerMap_.end()) {
        return;
    }
    lottieHandlerMap_.erase(it);
}

} // namespace LottieTurbo
