/*
 * Copyright (C) 2024 Huawei Device Co., Ltd.
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

#ifndef LOTTIETURBO_LOTTIEMANAGER_H
#define LOTTIETURBO_LOTTIEMANAGER_H

#include "cache/file_cache.h"
#include "common/common.h"
#include "controller/lottie_handler.h"
#include "node/lottie_manager_map.h"
#include "cangjie/lottie_xcomponent_node.h"
#include <ace/xcomponent/native_interface_xcomponent.h>
#include <arkui/native_node.h>
#include <arkui/native_type.h>
#include <map>
#include <string>
namespace LottieTurbo {

class LottieManager {
public:
    static LottieManager &GetInstance()
    {
        // 局部静态变量，只有在首次定义定义它的函数时才被构造，之后的调用都直接跳过，返回同一实例
        static LottieManager instance;
        return instance;
    }

    // 禁止拷贝构造和赋值运算符
    LottieManager(const LottieManager &) = delete;
    LottieManager &operator=(const LottieManager &) = delete;

    // 禁止移动构造和移位赋值运算符
    LottieManager(LottieManager &&) = delete;
    LottieManager &operator=(LottieManager &&) = delete;

    bool RepeatLottieId(std::string id)
    {
        auto it = contentHandleMap_.find(id);
        if (it != contentHandleMap_.end()) {
            DRAWING_LOGE("LottieManager::RepeatLottieId: RepeatLottieId");
            return true;
        }
        return false;
    }

    bool AddLottieNode(std::string id,
                       const std::shared_ptr<LottieXcomponentNode> &lottieXcomponentNode,
                       const std::shared_ptr<LottieHandler> &LottieHandler)
    {
        auto it = contentHandleMap_.find(id);
        if (it != contentHandleMap_.end()) {
            DRAWING_LOGE("LottieManager::AddLottieNode: Same LottieId");
            return false;
        }
        contentHandleMap_.insert({id, std::make_unique<ContentHandle>()});
        it = contentHandleMap_.find(id);
        it->second->lottieHandler = LottieHandler;

        it->second->lottieXcomponentNode = lottieXcomponentNode;

        if (!lottieXcomponentNode) {
            DRAWING_LOGE("LottieManager::AddLottieNode: Error OH_ArkUI_NodeContent_AddNode params NULL");
            return false;
        }
        DRAWING_LOGD("LottieManager::AddNode LottieId:%{public}s", id.c_str());
        // 组件添加完成回调
        if (it->second->lottieHandler->lottieListener_) {
            auto func = [](LottieListener *listener, LottieEventType type) {
                if (listener) {
                    listener->TriggerEvent(type);
                }
            };
            auto trigger = new LottieHandler::TriggerData;
            trigger->func =
                std::bind(func, it->second->lottieHandler->lottieListener_.get(), LottieEventType::ON_DOM_LOADED);

            it->second->lottieHandler->safeFunc_(trigger);
        }
        return true;
    }

    void DisposeLottieNode(std::string id)
    {
        auto it = contentHandleMap_.find(id);
        if (it == contentHandleMap_.end()) {
            return;
        }

        if (!it->second->lottieXcomponentNode) {
            DRAWING_LOGE("LottieManager::DisposeLottieNode:: Error OH_ArkUI_NodeContent_RemoveNode params NULL");
            contentHandleMap_.erase(id);
            return;
        }
        std::string tmpDir = it->second->lottieHandler->dataManager_->GetFilePath();
        // 这里停止动画后再下树清理资源
        it->second->lottieHandler->animator_->Stop();
        DRAWING_LOGD("LottieManager::DisposeLottieNode LottieId:%{public}s", id.c_str());

        LottieManagerMap::getInstance().Delete(id);
        // 清理组件的时候，把该组件产生的临时目录文件一起清除
        FileCache::GetInstance()->DeleteTempDir(tmpDir);
        // 组件销毁回调
        if (it->second->lottieHandler->lottieListener_) {
            it->second->lottieHandler->lottieListener_->TriggerEvent(LottieEventType::ON_DESTROY);
        }
        contentHandleMap_.erase(id);
    }

    std::shared_ptr<LottieHandler> GetHandleById(const std::string &id)
    {
        auto it = contentHandleMap_.find(id);
        if (it != contentHandleMap_.end()) {
            if (it->second && it->second->lottieHandler) {
                return it->second->lottieHandler;
            }
        }
        return std::shared_ptr<LottieHandler>(nullptr);
    }

    std::shared_ptr<LottieXcomponentNode> GetXcomponentById(const std::string &id)
    {
        auto it = contentHandleMap_.find(id);
        if (it != contentHandleMap_.end()) {
            if (it->second && it->second->lottieXcomponentNode) {
                return it->second->lottieXcomponentNode;
            }
        }
        return std::shared_ptr<LottieXcomponentNode>(nullptr);
    }

    std::vector<std::shared_ptr<LottieHandler>> GetLottieHandlerList()
    {
        std::vector<std::shared_ptr<LottieHandler>> LottieHandlerList;
        for (auto it = contentHandleMap_.begin(); it != contentHandleMap_.end(); ++it) {
            if (it->second->lottieHandler) {
                LottieHandlerList.emplace_back(it->second->lottieHandler);
            } else {
                std::string name = it->first;
                DRAWING_LOGD("GetLottieHandlerList::Lottie handler for %{public}s is nullptr.", name.c_str());
            }
        }
        return LottieHandlerList;
    }

private:
    LottieManager()
    {
    }
    ~LottieManager()
    {
    }
    typedef struct {
        std::shared_ptr<LottieXcomponentNode> lottieXcomponentNode;
        ArkUI_NodeContentHandle contentHandle;
        std::shared_ptr<LottieHandler> lottieHandler;
    } ContentHandle;

    std::map<std::string, std::unique_ptr<ContentHandle>> contentHandleMap_;
};
} // namespace LottieTurbo

#endif
