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

#include "round_rect.h"

namespace LottieTurbo {
namespace drawing {

RoundRect::RoundRect(Rect rect, float rx, float ry)
    : rect_(std::move(rect)),
      roundRect_(UniqueNativeRoundRect(OH_Drawing_RoundRectCreate(rect_.get(), rx, ry), &OH_Drawing_RoundRectDestroy))
{
}
void RoundRect::SetCorner(CornerPos pos, CornerRadii radii)
{
    OH_Drawing_RoundRectSetCorner(roundRect_.get(), pos, radii);
}
RoundRect::CornerRadii RoundRect::GetCorner(CornerPos pos)
{
    return OH_Drawing_RoundRectGetCorner(roundRect_.get(), pos);
}

} // namespace drawing
} // namespace LottieTurbo