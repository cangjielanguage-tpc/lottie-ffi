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

#ifndef LOTTIETURBO_CANVAS_H
#define LOTTIETURBO_CANVAS_H

#include "bitmap.h"
#include "brush.h"
#include "matrix.h"
#include "path.h"
#include "pen.h"
#include <memory>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_types.h>

namespace LottieTurbo::drawing {

class Canvas final {
    // 使用智能指针管理native canvas资源
    using UniqueNativeCanvas = std::unique_ptr<OH_Drawing_Canvas, decltype(&OH_Drawing_CanvasDestroy)>;

public:
    // 类型别名定义
    using ClipOp = OH_Drawing_CanvasClipOp;           // 裁剪操作类型
    using PointMode = OH_Drawing_PointMode;           // 点绘制模式
    using ShadowFlags = OH_Drawing_CanvasShadowFlags; // 阴影标志
    using Point = OH_Drawing_Point2D;                 // 2D点
    using BlendMode = OH_Drawing_BlendMode;           // 混合模式
    using VertexMode = OH_Drawing_VertexMode;         // 顶点模式

    // 顶点绘制参数结构体
    struct VerticesParams {
        VertexMode mode;                // 顶点模式
        int32_t vertexCount;            // 顶点数量
        const Point *positions;         // 顶点位置数组
        const Point *texs;              // 纹理坐标数组
        const uint32_t *colors;         // 颜色数组
        int32_t indexCount;             // 索引数量
        const uint16_t *indices;        // 索引数组
        BlendMode blendMode;            // 混合模式
    };

    // 阴影绘制参数结构体
    struct ShadowParams {
        OH_Drawing_Path *path;          // 路径
        OH_Drawing_Point3D planeParams; // 平面参数
        OH_Drawing_Point3D devLightPos; // 光源位置
        float lightRadius;              // 光源半径
        uint32_t ambientColor;          // 环境光颜色
        uint32_t spotColor;             // 聚光灯颜色
        ShadowFlags flags;              // 阴影标志
    };

    explicit Canvas(OH_Drawing_Canvas *canvas)
        : canvas_(UniqueNativeCanvas(canvas, [](OH_Drawing_Canvas *) {
        /* 空析构函数 */
        }))
    {
    }
    // 构造函数和析构函数
    Canvas();
    ~Canvas() noexcept = default;

    // 禁用拷贝
    Canvas(Canvas const &) = delete;
    Canvas &operator=(Canvas const &) = delete;

    // 启用移动
    Canvas(Canvas &&) = default;
    Canvas &operator=(Canvas &&) = default;

    // 画笔相关操作
    void AttachPen(const Pen &pen);       // 附加画笔
    void DetachPen();                     // 分离画笔
    void AttachBrush(const Brush &brush); // 附加画刷
    void DetachBrush();                   // 分离画刷

    // 基本绘图操作
    void DrawPath(const OH_Drawing_Path *path);                                    // 绘制路径
    void DrawLine(float x1, float y1, float x2, float y2);                         // 绘制直线
    void DrawPoint(const Point *point);                                            // 绘制点
    void DrawPoints(PointMode mode, uint32_t count, const Point *points);          // 绘制多个点
    void DrawRect(const OH_Drawing_Rect *rect);                                    // 绘制矩形
    void DrawRoundRect(const OH_Drawing_RoundRect *roundRect);                     // 绘制圆角矩形
    void DrawCircle(const OH_Drawing_Point *center, float radius);                 // 绘制圆
    void DrawOval(const OH_Drawing_Rect *rect);                                    // 绘制椭圆
    void DrawArc(const OH_Drawing_Rect *rect, float startAngle, float sweepAngle); // 绘制圆弧
    void BindBitmap(Bitmap *bitmap);
    // 文本绘制
    void DrawSingleChar(const char *str, const OH_Drawing_Font *font, float x, float y); // 绘制单个字符
    void DrawTextBlob(const OH_Drawing_TextBlob *textBlob, float x, float y);            // 绘制文本块

    // 图像绘制
    void DrawBitmap(const OH_Drawing_Bitmap *bitmap, float left, float top);         // 绘制位图
    void DrawBitmapRect(const OH_Drawing_Bitmap *bitmap, const OH_Drawing_Rect *src, // 绘制位图区域
                        const OH_Drawing_Rect *dst, const OH_Drawing_SamplingOptions *sampling);

    // 裁剪操作
    void ClipRect(const OH_Drawing_Rect *rect, ClipOp op, bool antiAlias);           // 矩形裁剪
    void ClipRoundRect(const OH_Drawing_RoundRect *rect, ClipOp op, bool antiAlias); // 圆角矩形裁剪
    void ClipPath(const OH_Drawing_Path *path, ClipOp op, bool antiAlias);           // 路径裁剪
    void ClipRegion(const OH_Drawing_Region *region, ClipOp op);                     // 区域裁剪

    // 变换操作
    void Save();                                                                // 保存画布状态
    void SaveLayer(const OH_Drawing_Rect *rect, const OH_Drawing_Brush *brush); // 保存图层
    void Restore();                                                             // 恢复画布状态
    void RestoreToCount(uint32_t count);                                        // 恢复到指定状态
    void Translate(float dx, float dy);                                         // 平移
    void Scale(float sx, float sy);                                             // 缩放
    void Rotate(float degrees, float px, float py);                             // 旋转
    void Skew(float sx, float sy);                                              // 倾斜
    void ConcatMatrix(OH_Drawing_Matrix *matrix);                               // 连接矩阵
    void SetMatrix(OH_Drawing_Matrix *matrix);                                  // 设置矩阵
    void ResetMatrix();                                                         // 重置矩阵

    // 状态查询
    uint32_t GetSaveCount() const;                             // 获取保存状态数量
    int32_t GetWidth() const;                                  // 获取宽度
    int32_t GetHeight() const;                                 // 获取高度
    void GetLocalClipBounds(OH_Drawing_Rect *rect) const;      // 获取局部裁剪边界
    void GetTotalMatrix(OH_Drawing_Matrix *matrix) const;      // 获取总变换矩阵
    bool IsClipEmpty() const;                                  // 检查裁剪区域是否为空
    void GetImageInfo(OH_Drawing_Image_Info *imageInfo) const; // 获取图像信息

    // 像素操作
    bool ReadPixels(OH_Drawing_Image_Info *imageInfo, void *dstPixels, // 读取像素数据
                    uint32_t dstRowBytes, int32_t srcX, int32_t srcY);
    bool ReadPixelsToBitmap(OH_Drawing_Bitmap *bitmap,                 // 读取像素到位图
                            int32_t srcX, int32_t srcY);

    // 其他操作
    void Clear(uint32_t color);                             // 清除画布
    void DrawColor(uint32_t color, BlendMode blendMode);    // 绘制颜色
    void DrawBackground(const OH_Drawing_Brush *brush);     // 绘制背景
    void DrawRegion(const OH_Drawing_Region *region);       // 绘制区域
    void DrawVertices(const VerticesParams &params);        // 绘制顶点
    void DrawShadow(const ShadowParams &params);            // 绘制阴影

    // 获取原生canvas对象
    OH_Drawing_Canvas *get() const
    {
        return canvas_.get();
    }

private:
    UniqueNativeCanvas canvas_; // 原生画布对象
};

} // namespace LottieTurbo::drawing

#endif