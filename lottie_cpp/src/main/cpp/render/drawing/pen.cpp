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

#include "pen.h"
#include <native_drawing/drawing_path_effect.h>

namespace LottieTurbo::drawing {

Pen::Pen() : pen_(UniqueNativePen(OH_Drawing_PenCreate(), &OH_Drawing_PenDestroy))
{
}

Pen::Pen(Pen const &other) : pen_(UniqueNativePen(OH_Drawing_PenCopy(other.pen_.get()), &OH_Drawing_PenDestroy))
{
}

Pen &Pen::operator=(Pen const &)
{
    pen_ = UniqueNativePen(OH_Drawing_PenCopy(pen_.get()), &OH_Drawing_PenDestroy);
    return *this;
}

void Pen::SetAntiAlias(bool antiAlias)
{
    OH_Drawing_PenSetAntiAlias(pen_.get(), antiAlias);
}

void Pen::SetColor(uint32_t color)
{
    OH_Drawing_PenSetColor(pen_.get(), color);
}

void Pen::SetAlpha(uint8_t alpha)
{
    OH_Drawing_PenSetAlpha(pen_.get(), alpha);
}

void Pen::SetWidth(double width)
{
    OH_Drawing_PenSetWidth(pen_.get(), width);
}

void Pen::SetMiterLimit(float miterLimit)
{
    OH_Drawing_PenSetMiterLimit(pen_.get(), miterLimit);
}

void Pen::SetLineCap(LineCapStyle lineCap)
{
    OH_Drawing_PenSetCap(pen_.get(), lineCap);
}

void Pen::SetLineJoin(LineJoinStyle lineJoin)
{
    OH_Drawing_PenSetJoin(pen_.get(), lineJoin);
}

void Pen::SetDashPathEffect(float *intervals, int count, float phase)
{
    OH_Drawing_PathEffect *pathEffect = OH_Drawing_CreateDashPathEffect(intervals, count, phase);
    OH_Drawing_PenSetPathEffect(pen_.get(), pathEffect);
    OH_Drawing_PathEffectDestroy(pathEffect);
}

void Pen::SetLinearShaderEffect(const LinearShaderParams &params)
{
    penShaderEffect_.ShaderEffectCreateLinearGradient(params);
    OH_Drawing_PenSetShaderEffect(pen_.get(), penShaderEffect_.get());
}

void Pen::SetRadialShaderEffect(const RadialShaderParams &params)
{
    penShaderEffect_.ShaderEffectCreateRadialGradient(params);
    OH_Drawing_PenSetShaderEffect(pen_.get(), penShaderEffect_.get());
}

void Pen::SetImageShaderEffect(const ImageShaderParams &params)
{
    penShaderEffect_.ShaderEffectCreateImageShader(params);
    OH_Drawing_PenSetShaderEffect(pen_.get(), penShaderEffect_.get());
}

void Pen::SetFilter(OH_Drawing_Filter *filter)
{
    OH_Drawing_PenSetFilter(pen_.get(), filter);
}

void Pen::Reset()
{
}

} // namespace LottieTurbo::drawing