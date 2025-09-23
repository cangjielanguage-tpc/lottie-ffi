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

#ifndef LOTTIETURBO_BRUSH_H
#define LOTTIETURBO_BRUSH_H

#include "shader_effect.h"
#include <memory>
#include <native_drawing/drawing_brush.h>

namespace LottieTurbo::drawing {

class Brush final {
    using UniqueNativeBrush = std::unique_ptr<OH_Drawing_Brush, decltype(&OH_Drawing_BrushDestroy)>;

public:
    Brush();

    Brush(Brush const &);
    Brush &operator=(Brush const &);

    Brush(Brush &&) = default;
    Brush &operator=(Brush &&) = default;

    ~Brush() noexcept = default;

    void SetAntiAlias(bool antiAlias);
    void SetColor(uint32_t color);
    void SetAlpha(uint8_t alpha);
    void SetBlendMode(OH_Drawing_BlendMode blendMode);

    void SetLinearShaderEffect(const LinearShaderParams &params);
    void SetRadialShaderEffect(const RadialShaderParams &params);
    void SetImageShaderEffect(const ImageShaderParams &params);

    void SetShaderEffect(OH_Drawing_ShaderEffect *shaderEffect);

    void SetEffectFilter(OH_Drawing_Filter *filter);

    void Reset();

    uint32_t GetColor();

    OH_Drawing_Brush *get() const
    {
        return brush_.get();
    }

private:
    UniqueNativeBrush brush_;
    ShaderEffect brushShaderEffect_;
};

} // namespace LottieTurbo::drawing

#endif