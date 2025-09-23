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

#ifndef LOTTIETURBO_PATH_H
#define LOTTIETURBO_PATH_H

#include "matrix.h"
#include "rect.h"
#include "round_rect.h"
#include <memory>
#include <native_drawing/drawing_path.h>
#include <native_drawing/drawing_types.h>

namespace LottieTurbo::drawing {

// 弧线参数结构体
struct ArcParams {
    float x1;
    float y1;
    float x2;
    float y2;
    float startDeg;
    float sweepDeg;
};

// 三次贝塞尔曲线参数结构体
struct CubicParams {
    float ctrlX1;
    float ctrlY1;
    float ctrlX2;
    float ctrlY2;
    float endX;
    float endY;
};

// 二次贝塞尔曲线参数结构体
struct QuadParams {
    float ctrlX;
    float ctrlY;
    float endX;
    float endY;
};

// 圆锥曲线参数结构体
struct ConicParams {
    float ctrlX;
    float ctrlY;
    float endX;
    float endY;
    float weight;
};

class Path final {
    using UniqueNativePath = std::unique_ptr<OH_Drawing_Path, decltype(&OH_Drawing_PathDestroy)>;

public:
    using Direction = OH_Drawing_PathDirection;
    using AddMode = OH_Drawing_PathAddMode;
    using FillType = OH_Drawing_PathFillType;
    using MeasureMatrixFlags = OH_Drawing_PathMeasureMatrixFlags;
    using OpMode = OH_Drawing_PathOpMode;

    Path();

    Path(Path const &);
    Path &operator=(Path const &);

    Path(Path &&) = default;
    Path &operator=(Path &&) = default;

    ~Path() noexcept = default;

    void Reset();

    void MoveTo(float x, float y);

    void LineTo(float x, float y);

    void ArcTo(const ArcParams &params);

    void QuadTo(const QuadParams &params);

    void ConicTo(const ConicParams &params);

    void CubicTo(const CubicParams &params);

    void RMoveTo(float x, float y);

    void RLineTo(float x, float y);

    void RQuadTo(const QuadParams &params);

    void RConicTo(const ConicParams &params);

    void RCubicTo(const CubicParams &params);

    void AddRect(float left, float top, float right, float bottom, Direction);
    void AddRectWithInitialCorner(const Rect &, Direction, uint32_t start);
    void AddRoundRect(const RoundRect &roundRect, Direction);

    void AddOvalWithInitialPoint(const Rect &, uint32_t start, Direction);
    void AddOval(const Rect &, Direction);

    void AddArc(const Rect &, float startAngle, float sweepAngle);

    void AddPath(const Path &src);
    void AddPath(const Path &src, const Matrix &);

    void AddPathWithMatrixAndMode(const Path &src, const Matrix &, AddMode);
    void AddPathWithMode(const Path &src, AddMode);

    void AddPathWithOffsetAndMode(const Path &src, float dx, float dy, AddMode);
    void AddPolygon(const OH_Drawing_Point2D *points, uint32_t count, bool isClosed);

    void AddCircle(float x, float y, float radius, Direction);

    static std::optional<Path> BuildFromSvgString(const char *str);

    bool Contains(float x, float y);

    void Transform(const Matrix &);

    Path TransformWithPerspectiveClip(const Matrix &, bool applyPerspectiveClip);

    void SetFillType(FillType);

    float GetLength(bool forceClosed);

    Rect GetBounds();

    void Close();

    Path Offset(float dx, float dy);

    bool IsClosed(bool forceClosed);

    bool GetPositionTangent(bool forceClosed, float distance, OH_Drawing_Point2D *position,
                            OH_Drawing_Point2D *tangent);

    bool Op(Path &source, OpMode mode);

    bool Difference(Path &source);

    bool Intersect(Path &source);

    bool Union(Path &source);

    bool Xor(Path &source);

    bool ReverseDifference(Path &source);

    std::optional<Matrix> GetMatrix(bool forceClosed, float distance, MeasureMatrixFlags flag);

    OH_Drawing_Path *get() const
    {
        return path_.get();
    }

private:
    UniqueNativePath path_;
};

} // namespace LottieTurbo::drawing

#endif