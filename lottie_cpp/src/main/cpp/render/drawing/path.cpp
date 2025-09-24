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

#include "path.h"

namespace LottieTurbo {
namespace drawing {

Path::Path() : path_(UniqueNativePath(OH_Drawing_PathCreate(), &OH_Drawing_PathDestroy))
{
}

Path::Path(Path const &other) : path_(UniqueNativePath(OH_Drawing_PathCopy(other.path_.get()), &OH_Drawing_PathDestroy))
{
}

Path &Path::operator=(Path const &other)
{
    if (this != &other) {
        path_ = UniqueNativePath(OH_Drawing_PathCopy(other.path_.get()), &OH_Drawing_PathDestroy);
    }
    return *this;
}

void Path::Reset()
{
    OH_Drawing_PathReset(path_.get());
}

void Path::MoveTo(float x, float y)
{
    OH_Drawing_PathMoveTo(get(), x, y);
}

void Path::LineTo(float x, float y)
{
    OH_Drawing_PathLineTo(get(), x, y);
}

void Path::ArcTo(const ArcParams &params)
{
    OH_Drawing_PathArcTo(get(), params.x1, params.y1, params.x2, params.y2, params.startDeg, params.sweepDeg);
}

void Path::QuadTo(const QuadParams &params)
{
    OH_Drawing_PathQuadTo(get(), params.ctrlX, params.ctrlY, params.endX, params.endY);
}

void Path::ConicTo(const ConicParams &params)
{
    OH_Drawing_PathConicTo(get(), params.ctrlX, params.ctrlY, params.endX, params.endY, params.weight);
}

void Path::CubicTo(const CubicParams &params)
{
    OH_Drawing_PathCubicTo(get(), params.ctrlX1, params.ctrlY1, params.ctrlX2, params.ctrlY2, params.endX, params.endY);
}

void Path::RMoveTo(float x, float y)
{
    OH_Drawing_PathRMoveTo(get(), x, y);
}

void Path::RLineTo(float x, float y)
{
    OH_Drawing_PathRLineTo(get(), x, y);
}

void Path::RQuadTo(const QuadParams &params)
{
    OH_Drawing_PathRQuadTo(get(), params.ctrlX, params.ctrlY, params.endX, params.endY);
}

void Path::RConicTo(const ConicParams &params)
{
    OH_Drawing_PathRConicTo(get(), params.ctrlX, params.ctrlY, params.endX, params.endY, params.weight);
}

void Path::RCubicTo(const CubicParams &params)
{
    OH_Drawing_PathRCubicTo(get(), params.ctrlX1, params.ctrlY1, params.ctrlX2, params.ctrlY2, params.endX,
                            params.endY);
}

void Path::AddRect(float left, float top, float right, float bottom, Direction direction)
{
    OH_Drawing_PathAddRect(get(), left, top, right, bottom, direction);
}

void Path::AddRectWithInitialCorner(const Rect &rect, Direction direction, uint32_t start)
{
    OH_Drawing_PathAddRectWithInitialCorner(get(), rect.get(), direction, start);
}

void Path::AddRoundRect(const RoundRect &roundRect, Direction direction)
{
    OH_Drawing_PathAddRoundRect(get(), roundRect.get(), direction);
}

void Path::AddOvalWithInitialPoint(const Rect &rect, uint32_t start, Direction direction)
{
    OH_Drawing_PathAddOvalWithInitialPoint(get(), rect.get(), start, direction);
}

void Path::AddOval(const Rect &rect, Direction direction)
{
    OH_Drawing_PathAddOval(get(), rect.get(), direction);
}

void Path::AddArc(const Rect &rect, float startAngle, float sweepAngle)
{
    OH_Drawing_PathAddArc(get(), rect.get(), startAngle, sweepAngle);
}

void Path::AddPath(const Path &src)
{
    OH_Drawing_PathAddPath(get(), src.get(), nullptr);
}

void Path::AddPath(const Path &src, const Matrix &matrix)
{
    OH_Drawing_PathAddPath(get(), src.get(), matrix.get());
}

void Path::AddPathWithMatrixAndMode(const Path &src, const Matrix &matrix, AddMode mode)
{
    OH_Drawing_PathAddPathWithMatrixAndMode(get(), src.get(), matrix.get(), mode);
}

void Path::AddPathWithMode(const Path &src, AddMode mode)
{
    OH_Drawing_PathAddPathWithMode(get(), src.get(), mode);
}

void Path::AddPathWithOffsetAndMode(const Path &src, float dx, float dy, AddMode mode)
{
    OH_Drawing_PathAddPathWithOffsetAndMode(get(), src.get(), dx, dy, mode);
}

void Path::AddPolygon(const OH_Drawing_Point2D *points, uint32_t count, bool isClosed)
{
    OH_Drawing_PathAddPolygon(get(), points, count, isClosed);
}

void Path::AddCircle(float x, float y, float radius, Direction direction)
{
    OH_Drawing_PathAddCircle(get(), x, y, radius, direction);
}

std::optional<Path> Path::BuildFromSvgString(const char *str)
{
    Path path;
    if (OH_Drawing_PathBuildFromSvgString(path.get(), str)) {
        return path;
    } else {
        return std::nullopt;
    }
}

bool Path::Contains(float x, float y)
{
    return OH_Drawing_PathContains(get(), x, y);
}

void Path::Transform(const Matrix &matrix)
{
    OH_Drawing_PathTransform(get(), matrix.get());
}

Path Path::TransformWithPerspectiveClip(const Matrix &matrix, bool applyPerspectiveClip)
{
    Path path;
    OH_Drawing_PathTransformWithPerspectiveClip(get(), matrix.get(), path.get(), applyPerspectiveClip);
    return path;
}

void Path::SetFillType(FillType fillType)
{
    OH_Drawing_PathSetFillType(get(), fillType);
}

float Path::GetLength(bool forceClosed)
{
    return OH_Drawing_PathGetLength(get(), forceClosed);
}

Rect Path::GetBounds()
{
    Rect bounds;
    OH_Drawing_PathGetBounds(get(), bounds.get());
    return bounds;
}

void Path::Close()
{
    OH_Drawing_PathClose(get());
}

Path Path::Offset(float dx, float dy)
{
    Path path;
    OH_Drawing_PathOffset(get(), path.get(), dx, dy);
    return path;
}

bool Path::IsClosed(bool forceClosed)
{
    return OH_Drawing_PathIsClosed(get(), forceClosed);
}

bool Path::GetPositionTangent(bool forceClosed, float distance, OH_Drawing_Point2D *position,
                              OH_Drawing_Point2D *tangent)
{
    return OH_Drawing_PathGetPositionTangent(get(), forceClosed, distance, position, tangent);
}

bool Path::Op(Path &source, OpMode mode)
{
    return OH_Drawing_PathOp(get(), source.get(), mode);
}

bool Path::Difference(Path &source)
{
    return Op(source, PATH_OP_MODE_DIFFERENCE);
}

bool Path::Intersect(Path &source)
{
    return Op(source, PATH_OP_MODE_INTERSECT);
}

bool Path::Union(Path &source)
{
    return Op(source, PATH_OP_MODE_UNION);
}

bool Path::Xor(Path &source)
{
    return Op(source, PATH_OP_MODE_XOR);
}

bool Path::ReverseDifference(Path &source)
{
    return Op(source, PATH_OP_MODE_REVERSE_DIFFERENCE);
}

std::optional<Matrix> Path::GetMatrix(bool forceClosed, float distance, MeasureMatrixFlags flag)
{
    Matrix matrix;
    if (OH_Drawing_PathGetMatrix(get(), forceClosed, distance, matrix.get(), flag)) {
        return matrix;
    } else {
        return std::nullopt;
    }
}

} // namespace drawing
} // namespace LottieTurbo