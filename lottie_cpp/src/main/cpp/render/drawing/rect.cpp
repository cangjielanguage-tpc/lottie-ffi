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

#include "rect.h"

namespace LottieTurbo {
namespace drawing {
Rect::Rect() : Rect(0, 0, 0, 0)
{
}

Rect::Rect(float left, float top, float right, float bottom)
    : rect_(UniqueNativeRect(OH_Drawing_RectCreate(left, top, right, bottom), &OH_Drawing_RectDestroy))
{
}

Rect::Rect(Rect const &other) : Rect()
{
    *this = other;
}
Rect &Rect::operator=(Rect const &other)
{
    OH_Drawing_RectCopy(other.get(), get());
    return *this;
}

bool Rect::Intersect(const Rect &other)
{
    return OH_Drawing_RectIntersect(get(), other.get());
}

bool Rect::Join(const Rect &other)
{
    return OH_Drawing_RectJoin(get(), other.get());
}

void Rect::SetLeft(float left)
{
    OH_Drawing_RectSetLeft(get(), left);
}

void Rect::SetTop(float top)
{
    OH_Drawing_RectSetTop(get(), top);
}

void Rect::SetRight(float right)
{
    OH_Drawing_RectSetRight(get(), right);
}

void Rect::SetBottom(float bottom)
{
    OH_Drawing_RectSetBottom(get(), bottom);
}

float Rect::GetLeft()
{
    return OH_Drawing_RectGetLeft(get());
}

float Rect::GetTop()
{
    return OH_Drawing_RectGetTop(get());
}

float Rect::GetRight()
{
    return OH_Drawing_RectGetRight(get());
}

float Rect::GetBottom()
{
    return OH_Drawing_RectGetBottom(get());
}

float Rect::GetHeight()
{
    return OH_Drawing_RectGetHeight(get());
}

float Rect::GetWidth()
{
    return OH_Drawing_RectGetWidth(get());
}

} // namespace drawing
} // namespace LottieTurbo