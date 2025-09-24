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

#include "canvas.h"

namespace LottieTurbo::drawing {

Canvas::Canvas() : canvas_(UniqueNativeCanvas(OH_Drawing_CanvasCreate(), &OH_Drawing_CanvasDestroy))
{
}

// 画笔操作
void Canvas::AttachPen(const Pen &pen)
{
    OH_Drawing_CanvasAttachPen(canvas_.get(), pen.get());
}

void Canvas::DetachPen()
{
    OH_Drawing_CanvasDetachPen(canvas_.get());
}

void Canvas::AttachBrush(const Brush &brush)
{
    OH_Drawing_CanvasAttachBrush(canvas_.get(), brush.get());
}

void Canvas::DetachBrush()
{
    OH_Drawing_CanvasDetachBrush(canvas_.get());
}

// 基本绘图操作
void Canvas::DrawPath(const OH_Drawing_Path *path)
{
    OH_Drawing_CanvasDrawPath(canvas_.get(), path);
}

void Canvas::DrawLine(float x1, float y1, float x2, float y2)
{
    OH_Drawing_CanvasDrawLine(canvas_.get(), x1, y1, x2, y2);
}

void Canvas::DrawPoint(const Point *point)
{
    OH_Drawing_CanvasDrawPoint(canvas_.get(), point);
}

void Canvas::DrawPoints(PointMode mode, uint32_t count, const Point *points)
{
    OH_Drawing_CanvasDrawPoints(canvas_.get(), mode, count, points);
}

void Canvas::DrawRect(const OH_Drawing_Rect *rect)
{
    OH_Drawing_CanvasDrawRect(canvas_.get(), rect);
}

void Canvas::DrawRoundRect(const OH_Drawing_RoundRect *roundRect)
{
    OH_Drawing_CanvasDrawRoundRect(canvas_.get(), roundRect);
}

void Canvas::DrawCircle(const OH_Drawing_Point *center, float radius)
{
    OH_Drawing_CanvasDrawCircle(canvas_.get(), center, radius);
}

void Canvas::DrawOval(const OH_Drawing_Rect *rect)
{
    OH_Drawing_CanvasDrawOval(canvas_.get(), rect);
}

void Canvas::DrawArc(const OH_Drawing_Rect *rect, float startAngle, float sweepAngle)
{
    OH_Drawing_CanvasDrawArc(canvas_.get(), rect, startAngle, sweepAngle);
}

void Canvas::BindBitmap(Bitmap *bitmap)
{
    OH_Drawing_CanvasBind(canvas_.get(), bitmap->get());
}

// 文本绘制
void Canvas::DrawSingleChar(const char *str, const OH_Drawing_Font *font, float x, float y)
{
    OH_Drawing_CanvasDrawSingleCharacter(canvas_.get(), str, font, x, y);
}

void Canvas::DrawTextBlob(const OH_Drawing_TextBlob *textBlob, float x, float y)
{
    OH_Drawing_CanvasDrawTextBlob(canvas_.get(), textBlob, x, y);
}

// 图像绘制
void Canvas::DrawBitmap(const OH_Drawing_Bitmap *bitmap, float left, float top)
{
    OH_Drawing_CanvasDrawBitmap(canvas_.get(), bitmap, left, top);
}

void Canvas::DrawBitmapRect(const OH_Drawing_Bitmap *bitmap, const OH_Drawing_Rect *src, const OH_Drawing_Rect *dst,
                            const OH_Drawing_SamplingOptions *sampling)
{
    OH_Drawing_CanvasDrawBitmapRect(canvas_.get(), bitmap, src, dst, sampling);
}

// 裁剪操作
void Canvas::ClipRect(const OH_Drawing_Rect *rect, ClipOp op, bool antiAlias)
{
    OH_Drawing_CanvasClipRect(canvas_.get(), rect, op, antiAlias);
}

void Canvas::ClipRoundRect(const OH_Drawing_RoundRect *rect, ClipOp op, bool antiAlias)
{
    OH_Drawing_CanvasClipRoundRect(canvas_.get(), rect, op, antiAlias);
}

void Canvas::ClipPath(const OH_Drawing_Path *path, ClipOp op, bool antiAlias)
{
    OH_Drawing_CanvasClipPath(canvas_.get(), path, op, antiAlias);
}

void Canvas::ClipRegion(const OH_Drawing_Region *region, ClipOp op)
{
    OH_Drawing_CanvasClipRegion(canvas_.get(), region, op);
}

// 变换操作
void Canvas::Save()
{
    OH_Drawing_CanvasSave(canvas_.get());
}

void Canvas::SaveLayer(const OH_Drawing_Rect *rect, const OH_Drawing_Brush *brush)
{
    OH_Drawing_CanvasSaveLayer(canvas_.get(), rect, brush);
}

void Canvas::Restore()
{
    OH_Drawing_CanvasRestore(canvas_.get());
}

void Canvas::RestoreToCount(uint32_t count)
{
    OH_Drawing_CanvasRestoreToCount(canvas_.get(), count);
}

void Canvas::Translate(float dx, float dy)
{
    OH_Drawing_CanvasTranslate(canvas_.get(), dx, dy);
}

void Canvas::Scale(float sx, float sy)
{
    OH_Drawing_CanvasScale(canvas_.get(), sx, sy);
}

void Canvas::Rotate(float degrees, float px, float py)
{
    OH_Drawing_CanvasRotate(canvas_.get(), degrees, px, py);
}

void Canvas::Skew(float sx, float sy)
{
    OH_Drawing_CanvasSkew(canvas_.get(), sx, sy);
}

void Canvas::ConcatMatrix(OH_Drawing_Matrix *matrix)
{
    OH_Drawing_CanvasConcatMatrix(canvas_.get(), matrix);
}

void Canvas::SetMatrix(OH_Drawing_Matrix *matrix)
{
    OH_Drawing_CanvasSetMatrix(canvas_.get(), matrix);
}

void Canvas::ResetMatrix()
{
    OH_Drawing_CanvasResetMatrix(canvas_.get());
}

// 状态查询
uint32_t Canvas::GetSaveCount() const
{
    return OH_Drawing_CanvasGetSaveCount(canvas_.get());
}

int32_t Canvas::GetWidth() const
{
    return OH_Drawing_CanvasGetWidth(canvas_.get());
}

int32_t Canvas::GetHeight() const
{
    return OH_Drawing_CanvasGetHeight(canvas_.get());
}

void Canvas::GetLocalClipBounds(OH_Drawing_Rect *rect) const
{
    OH_Drawing_CanvasGetLocalClipBounds(canvas_.get(), rect);
}

void Canvas::GetTotalMatrix(OH_Drawing_Matrix *matrix) const
{
    OH_Drawing_CanvasGetTotalMatrix(canvas_.get(), matrix);
}

bool Canvas::IsClipEmpty() const
{
    bool isEmpty = false;
    OH_Drawing_CanvasIsClipEmpty(canvas_.get(), &isEmpty);
    return isEmpty;
}

void Canvas::GetImageInfo(OH_Drawing_Image_Info *imageInfo) const
{
    OH_Drawing_CanvasGetImageInfo(canvas_.get(), imageInfo);
}

// 像素操作
bool Canvas::ReadPixels(OH_Drawing_Image_Info *imageInfo, void *dstPixels, uint32_t dstRowBytes, int32_t srcX,
                        int32_t srcY)
{
    return OH_Drawing_CanvasReadPixels(canvas_.get(), imageInfo, dstPixels, dstRowBytes, srcX, srcY);
}

bool Canvas::ReadPixelsToBitmap(OH_Drawing_Bitmap *bitmap, int32_t srcX, int32_t srcY)
{
    return OH_Drawing_CanvasReadPixelsToBitmap(canvas_.get(), bitmap, srcX, srcY);
}

// 其他操作
void Canvas::Clear(uint32_t color)
{
    OH_Drawing_CanvasClear(canvas_.get(), color);
}

void Canvas::DrawColor(uint32_t color, BlendMode blendMode)
{
    OH_Drawing_CanvasDrawColor(canvas_.get(), color, blendMode);
}

void Canvas::DrawBackground(const OH_Drawing_Brush *brush)
{
    OH_Drawing_CanvasDrawBackground(canvas_.get(), brush);
}

void Canvas::DrawRegion(const OH_Drawing_Region *region)
{
    OH_Drawing_CanvasDrawRegion(canvas_.get(), region);
}

void Canvas::DrawVertices(const VerticesParams &params)
{
    OH_Drawing_CanvasDrawVertices(canvas_.get(), params.mode, params.vertexCount, params.positions,
                                  params.texs, params.colors, params.indexCount, params.indices,
                                  params.blendMode);
}

void Canvas::DrawShadow(const ShadowParams &params)
{
    OH_Drawing_CanvasDrawShadow(canvas_.get(), params.path, params.planeParams, params.devLightPos,
                                params.lightRadius, params.ambientColor, params.spotColor, params.flags);
}

} // namespace LottieTurbo::drawing