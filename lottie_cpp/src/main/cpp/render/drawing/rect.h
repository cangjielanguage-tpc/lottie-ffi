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

#ifndef LOTTIETURBO_RECT_H
#define LOTTIETURBO_RECT_H

#include <memory>
#include <native_drawing/drawing_rect.h>

namespace LottieTurbo {
namespace drawing {
class Rect final {
    using UniqueNativeRect = std::unique_ptr<OH_Drawing_Rect, decltype(&OH_Drawing_RectDestroy)>;

public:
    Rect();
    Rect(float left, float top, float right, float bottom);
    Rect(Rect const &);
    Rect &operator=(Rect const &);
    Rect(Rect &&) = default;
    Rect &operator=(Rect &&) = default;
    ~Rect() noexcept = default;

    bool Intersect(const Rect &other);

    bool Join(const Rect &other);

    void SetLeft(float left);

    void SetTop(float top);

    void SetRight(float right);

    void SetBottom(float bottom);

    float GetLeft();

    float GetTop();

    float GetRight();

    float GetBottom();

    float GetHeight();

    float GetWidth();

    OH_Drawing_Rect *get() const
    {
        return rect_.get();
    }

private:
    UniqueNativeRect rect_;
};
} // namespace drawing
} // namespace LottieTurbo

#endif