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

#ifndef LOTTIETURBO_SHADER_EFFECT_H
#define LOTTIETURBO_SHADER_EFFECT_H

#include <memory>
#include <native_drawing/drawing_matrix.h>
#include <native_drawing/drawing_point.h>
#include <native_drawing/drawing_shader_effect.h>
#include <native_drawing/drawing_types.h>

namespace LottieTurbo::drawing {

// 线性渐变着色器参数结构体
struct LinearShaderParams {
    const OH_Drawing_Point2D *startPt;    // 起点
    const OH_Drawing_Point2D *endPt;      // 终点
    const uint32_t *colors;               // 颜色数组
    const float *pos;                     // 位置数组
    uint32_t size;                        // 颜色和位置数组大小
    OH_Drawing_TileMode mode;             // 平铺模式
    const OH_Drawing_Matrix *mat;         // 变换矩阵
};

// 径向渐变着色器参数结构体
struct RadialShaderParams {
    const OH_Drawing_Point2D *startPt;    // 起点
    float startRadius;                    // 起始半径
    const OH_Drawing_Point2D *endPt;      // 终点
    float endRadius;                      // 结束半径
    const uint32_t *colors;               // 颜色数组
    const float *pos;                     // 位置数组
    uint32_t size;                        // 颜色和位置数组大小
    OH_Drawing_TileMode mode;             // 平铺模式
    const OH_Drawing_Matrix *mat;         // 变换矩阵
};

// 图像着色器参数结构体
struct ImageShaderParams {
    OH_Drawing_Image *image;               // 图像
    OH_Drawing_TileMode tileX;             // X轴平铺模式
    OH_Drawing_TileMode tileY;             // Y轴平铺模式
    const OH_Drawing_SamplingOptions *opt; // 采样选项
    const OH_Drawing_Matrix *mat;          // 变换矩阵
};

class ShaderEffect final {
    using UniqueNativeShaderEffect =
        std::unique_ptr<OH_Drawing_ShaderEffect, decltype(&OH_Drawing_ShaderEffectDestroy)>;
public:
    ShaderEffect();

    ShaderEffect(ShaderEffect const &other) = delete;
    ShaderEffect &operator=(ShaderEffect const &other) = delete;

    ShaderEffect(ShaderEffect &&other) = default;
    ShaderEffect &operator=(ShaderEffect &&other) = default;

    void ShaderEffectCreateLinearGradient(const LinearShaderParams &params);
    void ShaderEffectCreateRadialGradient(const RadialShaderParams &params);
    void ShaderEffectCreateImageShader(const ImageShaderParams &params);

    ~ShaderEffect() noexcept = default;

    OH_Drawing_ShaderEffect *get() const
    {
        return shaderEffect_.get();
    }

private:
    UniqueNativeShaderEffect shaderEffect_;
};

} // namespace LottieTurbo::drawing

#endif