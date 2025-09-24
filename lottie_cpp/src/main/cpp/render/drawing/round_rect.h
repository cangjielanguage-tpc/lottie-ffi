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

#ifndef LOTTIETURBO_ROUND_RECT_H
#define LOTTIETURBO_ROUND_RECT_H

#include "rect.h"
#include <memory>
#include <native_drawing/drawing_round_rect.h>

namespace LottieTurbo {
namespace drawing {
class RoundRect final {
    using UniqueNativeRoundRect = std::unique_ptr<OH_Drawing_RoundRect, decltype(&OH_Drawing_RoundRectDestroy)>;

    using CornerPos = OH_Drawing_CornerPos;
    using CornerRadii = OH_Drawing_Corner_Radii;

public:
    RoundRect(Rect rect, float rx, float ry);
    RoundRect(RoundRect const &) = delete;
    RoundRect &operator=(RoundRect const &) = delete;
    RoundRect(RoundRect &&) = default;
    RoundRect &operator=(RoundRect &&) = default;
    ~RoundRect() noexcept = default;

    void SetCorner(CornerPos pos, CornerRadii radii);
    CornerRadii GetCorner(CornerPos pos);

    OH_Drawing_RoundRect *get() const
    {
        return roundRect_.get();
    }

private:
    Rect rect_;
    UniqueNativeRoundRect roundRect_;
};
} // namespace drawing
} // namespace LottieTurbo

#endif