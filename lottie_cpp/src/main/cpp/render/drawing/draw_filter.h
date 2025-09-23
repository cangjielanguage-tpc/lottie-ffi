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

#ifndef LOTTIETURBO_DRAW_FILTER_H
#define LOTTIETURBO_DRAW_FILTER_H

#include <memory>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_types.h>
namespace LottieTurbo::drawing {

class DrawFilter final {
    using UniqueNativeDrawFilter = std::unique_ptr<OH_Drawing_Filter, decltype(&OH_Drawing_FilterDestroy)>;

public:
    // 构造和析构函数
    DrawFilter();
    ~DrawFilter() noexcept = default;

    // 禁用拷贝
    DrawFilter(const DrawFilter &) = delete;
    DrawFilter &operator=(const DrawFilter &) = delete;

    // 启用移动
    DrawFilter(DrawFilter &&) = default;
    DrawFilter &operator=(DrawFilter &&) = default;
    void SetFilter(float x, float y);
    void SetFilterSrgbMatrix(const float matrix[20]);
    void Destroy();
    OH_Drawing_Filter *getFilter() const
    {
        return draw_filter.get();
    }

private:
    UniqueNativeDrawFilter draw_filter;
    OH_Drawing_ImageFilter *imageFilter{nullptr};
};
} // namespace LottieTurbo::drawing

#endif // LOTTIETURBO_DRAW_FILTER_H
