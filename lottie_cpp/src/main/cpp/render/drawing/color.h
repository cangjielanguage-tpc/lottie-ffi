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

#ifndef LOTTIETURBO_COLOR_H
#define LOTTIETURBO_COLOR_H

#include <cstdint>
#include <native_drawing/drawing_color.h>

namespace LottieTurbo::drawing {

class Color final {
public:
    Color() = default;
    ~Color() noexcept = default;

    Color(Color const &) = default;
    Color &operator=(Color const &) = default;
    Color(Color &&) = default;
    Color &operator=(Color &&) = default;

    static uint32_t SetArgb(uint32_t alpha, uint32_t red, uint32_t green, uint32_t blue);
};

} // namespace LottieTurbo::drawing

#endif