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

#include "draw_filter.h"
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_filter.h>
#include <native_drawing/drawing_image_filter.h>
#include <native_drawing/drawing_shader_effect.h>

namespace LottieTurbo::drawing {
DrawFilter::DrawFilter() : draw_filter(UniqueNativeDrawFilter(OH_Drawing_FilterCreate(), &OH_Drawing_FilterDestroy))
{
}

void DrawFilter::SetFilter(float rx, float ry)
{
    imageFilter = OH_Drawing_ImageFilterCreateBlur(rx, ry, OH_Drawing_TileMode::DECAL, nullptr);
    OH_Drawing_FilterSetImageFilter(draw_filter.get(), imageFilter);
}

void DrawFilter::Destroy()
{
    if (imageFilter) {
        OH_Drawing_ImageFilterDestroy(imageFilter);
        imageFilter = nullptr;
    }
}
void DrawFilter::SetFilterSrgbMatrix(const float matrix[20])
{
    OH_Drawing_ColorFilter *colorMatrixFilter = OH_Drawing_ColorFilterCreateMatrix(matrix);
    OH_Drawing_ColorFilter *lumaFilter = OH_Drawing_ColorFilterCreateLinearToSrgbGamma();
    OH_Drawing_ColorFilter *colorFilter = OH_Drawing_ColorFilterCreateCompose(colorMatrixFilter, lumaFilter);
    OH_Drawing_FilterSetColorFilter(draw_filter.get(), colorFilter);
    OH_Drawing_ColorFilterDestroy(colorMatrixFilter);
    OH_Drawing_ColorFilterDestroy(lumaFilter);
}
}