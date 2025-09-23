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

#include "brush.h"

namespace LottieTurbo::drawing {

Brush::Brush() : brush_(UniqueNativeBrush(OH_Drawing_BrushCreate(), &OH_Drawing_BrushDestroy))
{
}

Brush::Brush(Brush const &other)
    : brush_(UniqueNativeBrush(OH_Drawing_BrushCopy(other.brush_.get()), &OH_Drawing_BrushDestroy))
{
}

Brush &Brush::operator=(Brush const &)
{
    brush_ = UniqueNativeBrush(OH_Drawing_BrushCopy(brush_.get()), &OH_Drawing_BrushDestroy);
    return *this;
}

void Brush::SetAntiAlias(bool antiAlias)
{
    OH_Drawing_BrushSetAntiAlias(brush_.get(), antiAlias);
}

void Brush::SetColor(uint32_t color)
{
    OH_Drawing_BrushSetColor(brush_.get(), color);
}

void Brush::SetBlendMode(OH_Drawing_BlendMode blendMode)
{
    OH_Drawing_BrushSetBlendMode(brush_.get(), blendMode);
}

void Brush::SetAlpha(uint8_t alpha)
{
    OH_Drawing_BrushSetAlpha(brush_.get(), alpha);
}

void Brush::SetLinearShaderEffect(const LinearShaderParams &params)
{
    brushShaderEffect_.ShaderEffectCreateLinearGradient(params);
    OH_Drawing_BrushSetShaderEffect(brush_.get(), brushShaderEffect_.get());
}

void Brush::SetRadialShaderEffect(const RadialShaderParams &params)
{
    brushShaderEffect_.ShaderEffectCreateRadialGradient(params);
    OH_Drawing_BrushSetShaderEffect(brush_.get(), brushShaderEffect_.get());
}

void Brush::SetImageShaderEffect(const ImageShaderParams &params)
{
    brushShaderEffect_.ShaderEffectCreateImageShader(params);
    OH_Drawing_BrushSetShaderEffect(brush_.get(), brushShaderEffect_.get());
}

void Brush::SetShaderEffect(OH_Drawing_ShaderEffect *shaderEffect)
{
    OH_Drawing_BrushSetShaderEffect(brush_.get(), shaderEffect);
}

uint32_t Brush::GetColor()
{
    return OH_Drawing_BrushGetColor(brush_.get());
}

void Brush::SetEffectFilter(OH_Drawing_Filter *filter)
{
    OH_Drawing_BrushSetFilter(brush_.get(), filter);
}

void Brush::Reset()
{
    OH_Drawing_BrushReset(brush_.get());
}

} // namespace LottieTurbo::drawing
