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

#include "shader_effect.h"

namespace LottieTurbo::drawing {

ShaderEffect::ShaderEffect() : shaderEffect_(UniqueNativeShaderEffect(nullptr, &OH_Drawing_ShaderEffectDestroy))
{
}

void ShaderEffect::ShaderEffectCreateLinearGradient(const LinearShaderParams &params)
{
    auto res = OH_Drawing_ShaderEffectCreateLinearGradientWithLocalMatrix(
        params.startPt, params.endPt, params.colors, params.pos, params.size, params.mode, params.mat);
    shaderEffect_ = UniqueNativeShaderEffect(res, &OH_Drawing_ShaderEffectDestroy);
}

void ShaderEffect::ShaderEffectCreateRadialGradient(const RadialShaderParams &params)
{
    auto res = OH_Drawing_ShaderEffectCreateTwoPointConicalGradient(
        params.startPt, params.startRadius, params.endPt, params.endRadius,
        params.colors, params.pos, params.size, params.mode, params.mat);
    shaderEffect_ = UniqueNativeShaderEffect(res, &OH_Drawing_ShaderEffectDestroy);
}

void ShaderEffect::ShaderEffectCreateImageShader(const ImageShaderParams &params)
{
    auto res = OH_Drawing_ShaderEffectCreateImageShader(
        params.image, params.tileX, params.tileY, params.opt, params.mat);
    shaderEffect_ = UniqueNativeShaderEffect(res, &OH_Drawing_ShaderEffectDestroy);
}

} // namespace LottieTurbo::drawing
