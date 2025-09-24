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

#ifndef LOTTIETURBO_MATRIX_H
#define LOTTIETURBO_MATRIX_H

#include "rect.h"
#include <memory>
#include <native_drawing/drawing_matrix.h>
#include <optional>
#include <vector>

namespace LottieTurbo::drawing {

// 矩阵参数结构体
struct MatrixParams {
    float scaleX;
    float skewX;
    float transX;
    float skewY;
    float scaleY;
    float transY;
    float persp0;
    float persp1;
    float persp2;
};

class Matrix final {
    using UniqueNativeMatrix = std::unique_ptr<OH_Drawing_Matrix, decltype(&OH_Drawing_MatrixDestroy)>;
    using Point = OH_Drawing_Point2D;

public:
    Matrix();

    Matrix(Matrix const &);
    Matrix &operator=(Matrix const &);

    Matrix(Matrix &&) = default;
    Matrix &operator=(Matrix &&) = default;

    ~Matrix() noexcept = default;

    static Matrix CreateRotation(float deg, float x, float y);
    static Matrix CreateScale(float sx, float sy, float px, float py);
    static Matrix CreateTranslation(float dx, float dy);
    void PreRotate(float degree, float px, float py);
    void PreScale(float sx, float sy, float px, float py);
    void PreTranslate(float dx, float dy);
    void PostRotate(float degree, float px, float py);
    void PostScale(float sx, float sy, float px, float py);
    void PostTranslate(float dx, float dy);
    void Rotate(float degree, float px, float py);
    void Translate(float dx, float dy);
    void Scale(float sx, float sy, float px, float py);

    Matrix Concat(const Matrix &other) const;
    std::optional<Matrix> Invert() const;

    float GetValue(int index) const;

    void SetMatrix(const MatrixParams &params);

    Point MapPoint(const Point &point) const;
    std::vector<Point> MapPoints(const std::vector<Point> &points) const;

    drawing::Rect MapRect(const drawing::Rect &rect) const;

    void Reset();

    OH_Drawing_Matrix *get() const
    {
        return matrix_.get();
    }

private:
    explicit Matrix(OH_Drawing_Matrix *rawMatrix) : matrix_(UniqueNativeMatrix(rawMatrix, &OH_Drawing_MatrixDestroy))
    {
    }

    UniqueNativeMatrix matrix_;
};

} // namespace LottieTurbo::drawing

#endif