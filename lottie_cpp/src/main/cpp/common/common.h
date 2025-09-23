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

#ifndef LOTTIETURBO_COMMON_H
#define LOTTIETURBO_COMMON_H
#include "hilog/log.h"
#include "hitrace/trace.h"
#include <atomic>
namespace LottieTurbo {

constexpr size_t LOG_PRINT_DOMAIN = 0xFF00;
constexpr size_t APP_LOG_DOMAIN = 0x0001;
constexpr const char *APP_LOG_TAG = "LottieTurbo";
constexpr size_t BUFF_SIZE = 1024;
#define DRAWING_LOGI(...) ((void)OH_LOG_Print(LOG_APP, LOG_INFO, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))
#define DRAWING_LOGD(...) ((void)OH_LOG_Print(LOG_APP, LOG_DEBUG, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))
#define DRAWING_LOGW(...) ((void)OH_LOG_Print(LOG_APP, LOG_WARN, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))
#define DRAWING_LOGE(...) ((void)OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, APP_LOG_TAG, __VA_ARGS__))
#if RELEASE_MODE
#define HITRACE(tag, exe)                                                                                              \
    do {                                                                                                               \
        exe;                                                                                                           \
    } while (0)
#else
#define HITRACE(tag, exe)                                                                                              \
    do {                                                                                                               \
        OH_HiTrace_StartAsyncTrace(tag, 0);                                                                            \
        exe;                                                                                                           \
        OH_HiTrace_FinishAsyncTrace(tag, 0);                                                                           \
    } while (0)
#endif

} // namespace LottieTurbo
#endif
