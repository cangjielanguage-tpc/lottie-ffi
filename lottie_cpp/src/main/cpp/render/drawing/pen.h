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

#ifndef LOTTIETURBO_PEN_H
#define LOTTIETURBO_PEN_H

#include "shader_effect.h"
#include <memory>
#include <native_drawing/drawing_pen.h>
#include <native_drawing/drawing_shader_effect.h>

namespace LottieTurbo::drawing {

class Pen final {
    using UniqueNativePen = std::unique_ptr<OH_Drawing_Pen, decltype(&OH_Drawing_PenDestroy)>;

public:
    using LineCapStyle = OH_Drawing_PenLineCapStyle;
    using LineJoinStyle = OH_Drawing_PenLineJoinStyle;

    Pen();

    Pen(Pen const &);
    Pen &operator=(Pen const &);

    Pen(Pen &&) = default;
    Pen &operator=(Pen &&) = default;

    ~Pen() noexcept = default;

    void SetAntiAlias(bool antiAlias);
    void SetColor(uint32_t color);
    void SetAlpha(uint8_t alpha);
    void SetWidth(double width);
    void SetMiterLimit(float miterLimit);
    void SetLineCap(LineCapStyle lineCap);
    void SetLineJoin(LineJoinStyle lineJoin);

    void SetLinearShaderEffect(const LinearShaderParams &params);
    void SetRadialShaderEffect(const RadialShaderParams &params);
    void SetImageShaderEffect(const ImageShaderParams &params);

    void SetFilter(OH_Drawing_Filter *filter);

    void SetDashPathEffect(float *intervals, int count, float phase);

    void Reset();

    OH_Drawing_Pen *get() const
    {
        return pen_.get();
    }

private:
    UniqueNativePen pen_;
    ShaderEffect penShaderEffect_;
};

} // namespace LottieTurbo::drawing

#endif