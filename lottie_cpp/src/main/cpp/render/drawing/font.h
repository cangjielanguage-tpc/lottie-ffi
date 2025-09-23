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

#ifndef LOTTIETURBO_FONT_H
#define LOTTIETURBO_FONT_H

#include <memory>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_typeface.h>

namespace LottieTurbo::drawing {

class Font final {
    // 使用智能指针管理native font资源
    using UniqueNativeFont = std::unique_ptr<OH_Drawing_Font, decltype(&OH_Drawing_FontDestroy)>;

public:
    // 类型别名定义
    using FontHinting = OH_Drawing_FontHinting;   // 字体微调模式
    using FontEdging = OH_Drawing_FontEdging;     // 字体边缘渲染模式
    using TextEncoding = OH_Drawing_TextEncoding; // 文本编码
    using FontMetrics = OH_Drawing_Font_Metrics;  // 字体度量信息

    // 构造函数和析构函数
    Font();
    ~Font() noexcept = default;

    // 拷贝构造和拷贝赋值
    Font(Font const &);
    Font &operator=(Font const &);

    // 移动构造和移动赋值
    Font(Font &&) = default;
    Font &operator=(Font &&) = default;

    // 字体属性设置与获取
    void SetTypeface(OH_Drawing_Typeface *typeface);
    OH_Drawing_Typeface *GetTypeface();

    void SetTextSize(float textSize);
    float GetTextSize() const;

    void SetLinearText(bool isLinearText);
    bool IsLinearText() const;

    void SetTextSkewX(float skewX);
    float GetTextSkewX() const;

    void SetFakeBoldText(bool isFakeBoldText);
    bool IsFakeBoldText() const;

    void SetScaleX(float scaleX);
    float GetScaleX() const;

    void SetHinting(FontHinting hinting);
    FontHinting GetHinting() const;

    void SetEmbeddedBitmaps(bool isEmbeddedBitmaps);
    bool IsEmbeddedBitmaps() const;

    void SetEdging(FontEdging edging);
    FontEdging GetEdging() const;

    void SetBaselineSnap(bool baselineSnap);
    bool IsBaselineSnap() const;

    void SetSubpixel(bool isSubpixel);
    bool IsSubpixel() const;

    void SetForceAutoHinting(bool isForceAutoHinting);
    bool IsForceAutoHinting() const;

    // 文本测量与操作方法
    int CountText(const void *text, size_t byteLength, TextEncoding encoding);

    uint32_t TextToGlyphs(const void *text, uint32_t byteLength, TextEncoding encoding, uint16_t *glyphs,
                          int maxGlyphCount);

    void GetWidths(const uint16_t *glyphs, int count, float *widths);

    bool MeasureSingleCharacter(const char *str, float *textWidth);

    bool MeasureText(const void *text, size_t byteLength, TextEncoding encoding, OH_Drawing_Rect *bounds,
                     float *textWidth);

    float GetMetrics(FontMetrics *metrics);

    // 获取原生font对象
    OH_Drawing_Font *get() const
    {
        return font_.get();
    }

private:
    UniqueNativeFont font_; // 原生字体对象
};

} // namespace LottieTurbo::drawing

#endif