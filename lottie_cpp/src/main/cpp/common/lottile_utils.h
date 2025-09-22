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

#ifndef LOTTIETURBO_LOTTILE_UTILS_H
#define LOTTIETURBO_LOTTILE_UTILS_H
namespace LottieTurbo {

static int DENSITY_DPI = 520; // 定义一个屏幕密度及其默认值
constexpr int DENSITY_PARAM = 160;
/**
 * vp具体计算公式为：vp= px/（DPI/160）
 */
inline int px2vp(int px)
{
    return px * DENSITY_PARAM / DENSITY_DPI;
}

} // namespace LottieTurbo
#endif // LOTTIETURBO_LOTTILE_UTILS_H
