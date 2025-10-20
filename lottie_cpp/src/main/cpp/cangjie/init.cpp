/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

#include "cache/file_cache.h"
#include "cache/memory_cache.h"
//#include "common/napi_helper.h"
#include "common/number_constant.h"
#include "cj_lottie_controller.h"
//#include "controller/lottie_controller_bridge.h"
#include "controller/lottie_handler.h"
#include "dataManager/lottie_config_data.h"
#include "hilog/log.h"
//#include "napi/native_api.h"
#include "node/lottie_manager.h"
#include "common/common.h"
#include "cangjie/lottie_xcomponent_node.h"
#include "rawfile/raw_file_manager.h"
#include "task_pool/task_pool.h"
#include <arkui/native_node_napi.h>
#include <cstdint>
#include <regex>

#include <stdlib.h>  // malloc/free
#include <string.h>  // memset
#include <stdint.h>  // uint8_t等类型
#include <stddef.h>  // size_t
/*
    ets侧调用接口
*/
namespace LottieTurbo {
extern "C" {
void LottieBasicAction(const char* lottieId,
                                    std::function<void(LottieTurbo::LottieHandler *, double value)> basicActionFunc,
                                    bool requiresValue = false,double speed = 0.0 )
{

    auto &lottieManager = LottieTurbo::LottieManager::GetInstance();
    auto handleList = lottieManager.GetLottieHandlerList();

    // 如果需要处理一个 double 值，先获取该值
    double value = requiresValue ? speed : 0.0;

    if (strcmp(lottieId, "") == 0) {
        for (auto &lottieHandler : handleList) {
            if (lottieHandler != nullptr) {
                basicActionFunc(lottieHandler.get(), value);
            }
        }
        return;
    }

    std::string name = lottieId;
    auto lottieHandler = LottieTurbo::LottieManager::GetInstance().GetHandleById(name);
    if (lottieHandler != nullptr) {
        basicActionFunc(lottieHandler.get(), value);
    } else {
        DRAWING_LOGE("LottieBasicAction: Lottie handler for %{public}s is nullptr.", name.c_str());
    }
    return;
}




void cj_LottieView_createNativeRoot(char *lottieId, LottieDataConfig lottieDataConfig)
{

    auto xcomponentId = lottieId;
    bool repeat = LottieManager::GetInstance().RepeatLottieId(xcomponentId);
    if (repeat) {
        return;
    }

    auto config = std::make_shared<LottieConfigBridge>(lottieDataConfig);
    auto node = std::make_shared<LottieXcomponentNode>(xcomponentId, config->GetRadius());

    //TODO:将 LottieXcomponentNode 实例存入到 CJLottieController 中，便于后续 onSurfaceCreated 触发，调用到 LottieXcomponentNode。
    intptr_t ptrInt = static_cast<intptr_t>(lottieDataConfig.controller);
    CJLottieController *controller = reinterpret_cast<CJLottieController *>(ptrInt);
    if (!controller) {
        OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "cj_LottieView_createNativeRoot", "The CJLottieController is null.");
        return;
    }
    controller->setLottieXcomponentNode(node);

    auto handler = std::make_shared<LottieHandler>(xcomponentId, config);
    handler->lottieListener_ = std::move(config->GetLottieListener());

    handler->safeFunc_ = std::move(config->GetSafeFunction());
    
    bool ret = LottieManager::GetInstance().AddLottieNode(xcomponentId, node, handler);
    if (ret == false) {
        return;
    }
    auto task = [](std::shared_ptr<LottieHandler> handler, std::shared_ptr<LottieXcomponentNode> node) {
        if (handler && node) {
            node->SetHandler(handler);
            node->SetAnimator();

            handler->InitHandleData();
            node->Init();
        }
    };
    TaskPool::GetInstance().PushTask(ffrt_queue_priority_high, task, handler, node);
    return;
}


 void lottiePlay(const char* lottieId)
{
    return LottieBasicAction(lottieId, [](LottieTurbo::LottieHandler *handler, double value) { handler->Play(); });
}

 void lottiePause(const char* lottieId)
{
    return LottieBasicAction(lottieId, [](LottieTurbo::LottieHandler *handler, double value) { handler->Pause(); });
}

 void lottieStop(const char* lottieId)
{
    return LottieBasicAction(lottieId, [](LottieTurbo::LottieHandler *handler, double value) { handler->Stop(); });
}

 void lottieSetSpeed(double speed, const char* lottieId)
{
    return LottieBasicAction(
        lottieId, [](LottieTurbo::LottieHandler *handler, double speed) { handler->SetSpeed(speed); }, true,speed);
}

 void lottieSetDirection(double direction, const char* lottieId)
{ 
    return LottieBasicAction(lottieId, [](LottieTurbo::LottieHandler *handler, double direction) { handler->SetDirection(direction); } , true,direction);
}

 void  lottieTogglePause(const char* lottieId)
{
    return LottieBasicAction(lottieId, [](LottieTurbo::LottieHandler *handler, double value) { handler->TogglePause(); });
}
//
 void HandleDisposeLottieNode(std::string id)
{
    auto handler = LottieManager::GetInstance().GetHandleById(id);
    if (handler == nullptr) {
        return;
    }
    auto func = [](std::string id) { LottieManager::GetInstance().DisposeLottieNode(id); };
    auto trigger = new LottieHandler::TriggerData;
    trigger->func = std::bind(func, id);;
    handler->safeFunc_(trigger);
}

 void lottieDestroy(const char* lottieIds)
{
    if (strcmp(lottieIds, "") == 0) {
        auto handleList = LottieTurbo::LottieManager::GetInstance().GetLottieHandlerList();
        for (auto &lottieHandler : handleList) {
            lottieHandler->isDestroy_ = true;
            TaskPoolLocal::GetInstance().PushTask(HandleDisposeLottieNode, lottieHandler->GetLottieId());
        }
    } else {
        std::string lottieId = lottieIds;
        auto handler = LottieTurbo::LottieManager::GetInstance().GetHandleById(lottieId);
        if (handler == nullptr) {
            return;
        }
        handler->isDestroy_ = true;
        TaskPoolLocal::GetInstance().PushTask(HandleDisposeLottieNode, lottieId);
    }
    return;
}

 void lottieSetFrameRate(double frameRate)
{
    auto basicActionFunc = [](LottieTurbo::LottieHandler *handler, double frameRate) {
        float maxFrameRate = handler->GetLottieFileTotalFrameRate();
        ArkUI_ExpectedFrameRateRange expectFrameRateRange;
        constexpr float frameRateLimits = 120;
        // 区间(1-----120)
        // 1. 设置帧率 = 0; 解除区间限制; 实际帧率
        // 2. 设置帧率 < 1; 区间(1-----120); frameRate = 1
        // 3. 1 < 实际帧率 < 设置帧率 < 120; 限制为源文件帧率
        // 4. 1 <= 设置帧率 <= 实际帧率 < 120;
        //      此时不需要修改，因为 frameRate 已经是用户指定的值
        // 5. 实际帧率 < 120 <= 设置帧率; 限制为源文件帧率
        // 6. 120 <= 实际帧率 <= 设置帧率 / 120 < 设置帧率 < 实际帧率; frameRate = 120
        if (frameRate == 0 || (maxFrameRate < frameRate && frameRate < frameRateLimits) ||
            (maxFrameRate < frameRateLimits && frameRate >= frameRateLimits)) {
            // 1,3,5条件
            frameRate = maxFrameRate;
        } else if (frameRate <= 0) {
            // 2条件
            return;
        } else if (frameRate >= frameRateLimits && maxFrameRate >= frameRateLimits) {
            // 6条件
            frameRate = frameRateLimits;
        }
        expectFrameRateRange = {.min = static_cast<uint32_t>(frameRate),
                                .max = static_cast<uint32_t>(frameRate),
                                .expected = static_cast<uint32_t>(frameRate)};
        //DRAWING_LOGD("lottieSetFrameRate::Final maxFrameRate : %{public}f, frameRate : %{public}f.", maxFrameRate,
        //             frameRate);
        handler->Pause();                                            // 暂停动画
        handler->SetFrameRate(frameRate);                            // 设置动画源帧率
        handler->animator_->SetFrameRateRange(expectFrameRateRange); // 设置帧率范围
        handler->animator_->UpdateFrameRateRange();                  // 更新帧率范围
        handler->Play();                                             // 播放动画
    };
    return LottieBasicAction("", basicActionFunc, true,frameRate);
}

 void lottieInitFileCache()
{
    FileCache::GetInstance()->InitFileCache();
    return;
}

 void lottieResizeFileCache(int64_t numbers, int64_t capacitys)
{

    size_t number = (size_t)numbers;
    size_t capacity = (size_t)capacitys;
    FileCache::GetInstance()->SetCacheSize(number, capacity);
    return;
}

 void lottieClearFileCache(const char* url)
{

    std::string str_url = url;
    if (str_url.size() == 0) {
        FileCache::GetInstance()->RemoveAllFile();
    } else {
        FileCache::GetInstance()->RemoveFile(str_url);
    }
    return;
}

 void  lottieResizeCache(int number, int capacity)
{

    MemoryCache::GetInstance()->SetMemoryCache(number, capacity);
    return;
}

 void lottieClearCache()
{
    MemoryCache::GetInstance()->RemoveAll();
//    return nullptr;
}

 void lottieRemoveCache(const char* path)
{
    std::string path_url = path;
    if (path_url.size() == NUM_1) {
        MemoryCache::GetInstance()->Remove(path_url);
    }
    return;
}

 bool lottieContainsCache(const char* path)
{

    bool result = false;
    std::string key = path;
    if (key.size() == LottieTurbo::NUM_1) {
        auto data = LottieTurbo::MemoryCache::GetInstance()->GetData(key);
        if (data == nullptr) {
            return result;
        }
        return true;
    }
    return result;
}

 void ConfigureCacheSize(int cacheSize)
{

    // 调用底层接口
    rlottie::configureModelCacheSize(cacheSize);

    return;
}

 void LottieSetVisible(const char* lottieIds, bool visible)
{

    std::string lottieId = lottieIds;
    auto lottieHandler = LottieTurbo::LottieManager::GetInstance().GetHandleById(lottieId);
    if (lottieHandler != nullptr) {
        auto state = lottieHandler->GetPlayState();
        if (state == LottieTurbo::LottieHandler::PlayState::PLAY && visible == false) {
            lottieHandler->SetPlayState(LottieTurbo::LottieHandler::PlayState::INVISIBLE_PAUSE);
            lottieHandler->Pause();
        }
        if (state == LottieTurbo::LottieHandler::PlayState::INVISIBLE_PAUSE && visible == true) {
            lottieHandler->SetPlayState(LottieTurbo::LottieHandler::PlayState::PLAY);
            lottieHandler->Play();
        }
        lottieHandler->dataManager_->SetVisible(visible);
        //OH_LOG_Print(LOG_APP, LOG_DEBUG, LOG_DOMAIN, "LottieSetVisible",
        //             "LottieId = %{public}s, cacheSize = %{public}d", lottieId.c_str(), visible);
    }
    return;
}


typedef struct {
    uint32_t* data;
    size_t size;
} ArrayBuffer;
// Resource c侧无法处理
ArrayBuffer  renderToImage(const char* lottie,double frameNum,double width,double height) {
    std::string lottieId = lottie;
    std::shared_ptr<DataManager> dataManager = std::make_shared<DataManager>();
    dataManager->InitFromJson(lottieId);
    if (dataManager->player == nullptr) {
        dataManager->InitFromFile(lottieId);
    }
    ArrayBuffer result = {NULL, 0};

    if (dataManager->player == nullptr) {
        DRAWING_LOGE("Lottie::RenderToImage, dataManager->player Null.");
        return result;
    }
    auto renderTree = dataManager->player->renderTree(frameNum, width, height);
    if (renderTree == nullptr) {
        return result;
    }
    std::shared_ptr<LottieRender> lottieRender = std::make_shared<LottieRender>();
    void *bitmap = malloc(width * height * RGBANUM);

    if (lottieRender->RenderToImage(renderTree, width, height, bitmap)) {
        DRAWING_LOGD("Lottie::RenderToImage000, RenderTree width:%{public}f,height:%{public}f", width, height);
        uint32_t* intPtr = reinterpret_cast<uint32_t*>(bitmap);
        result.data = intPtr;
        result.size = width * height;
        return result;
    }
    return result;
}


 void LottieClearTmpResource()
{
    // 检查是否存在动画句柄,如果存在则有可能正在使用tmp中的资源，这里不进行清除临时文件，避免异常
    if (LottieManager::GetInstance().GetLottieHandlerList().size() == 0) {
        FileCache::GetInstance()->ClearTmpFile();
    }
    return;
//    return nullptr;
}
}
}

