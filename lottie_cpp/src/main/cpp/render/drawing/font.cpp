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

#include "font.h"

namespace LottieTurbo::drawing {

Font::Font() : font_(UniqueNativeFont(OH_Drawing_FontCreate(), &OH_Drawing_FontDestroy))
{
}

Font::Font(Font const &other) : font_(UniqueNativeFont(OH_Drawing_FontCreate(), &OH_Drawing_FontDestroy))
{
    if (other.font_) {
        // 复制基本属性
        SetTextSize(OH_Drawing_FontGetTextSize(other.font_.get()));
        SetLinearText(OH_Drawing_FontIsLinearText(other.font_.get()));
        SetTextSkewX(OH_Drawing_FontGetTextSkewX(other.font_.get()));
        SetFakeBoldText(OH_Drawing_FontIsFakeBoldText(other.font_.get()));
        // 复制扩展属性
        SetScaleX(OH_Drawing_FontGetScaleX(other.font_.get()));
        SetHinting(OH_Drawing_FontGetHinting(other.font_.get()));
        SetEmbeddedBitmaps(OH_Drawing_FontIsEmbeddedBitmaps(other.font_.get()));
        SetEdging(OH_Drawing_FontGetEdging(other.font_.get()));
        SetBaselineSnap(OH_Drawing_FontIsBaselineSnap(other.font_.get()));
        SetSubpixel(OH_Drawing_FontIsSubpixel(other.font_.get()));
        SetForceAutoHinting(OH_Drawing_FontIsForceAutoHinting(other.font_.get()));
        // 复制字体类型
        if (OH_Drawing_FontGetTypeface(other.font_.get())) {
            SetTypeface(OH_Drawing_FontGetTypeface(other.font_.get()));
        }
    }
}

Font &Font::operator=(Font const &other)
{
    if (this != &other) {
        font_ = UniqueNativeFont(OH_Drawing_FontCreate(), &OH_Drawing_FontDestroy);
        if (other.font_) {
            // 复制基本属性
            SetTextSize(OH_Drawing_FontGetTextSize(other.font_.get()));
            SetLinearText(OH_Drawing_FontIsLinearText(other.font_.get()));
            SetTextSkewX(OH_Drawing_FontGetTextSkewX(other.font_.get()));
            SetFakeBoldText(OH_Drawing_FontIsFakeBoldText(other.font_.get()));

            // 复制扩展属性
            SetScaleX(OH_Drawing_FontGetScaleX(other.font_.get()));
            SetHinting(OH_Drawing_FontGetHinting(other.font_.get()));
            SetEmbeddedBitmaps(OH_Drawing_FontIsEmbeddedBitmaps(other.font_.get()));
            SetEdging(OH_Drawing_FontGetEdging(other.font_.get()));
            SetBaselineSnap(OH_Drawing_FontIsBaselineSnap(other.font_.get()));
            SetSubpixel(OH_Drawing_FontIsSubpixel(other.font_.get()));
            SetForceAutoHinting(OH_Drawing_FontIsForceAutoHinting(other.font_.get()));

            // 复制字体类型
            if (OH_Drawing_FontGetTypeface(other.font_.get())) {
                SetTypeface(OH_Drawing_FontGetTypeface(other.font_.get()));
            }
        }
    }
    return *this;
}

// 已有方法实现
void Font::SetTypeface(OH_Drawing_Typeface *typeface)
{
    OH_Drawing_FontSetTypeface(font_.get(), typeface);
}

OH_Drawing_Typeface *Font::GetTypeface()
{
    return OH_Drawing_FontGetTypeface(font_.get());
}

void Font::SetTextSize(float textSize)
{
    OH_Drawing_FontSetTextSize(font_.get(), textSize);
}

float Font::GetTextSize() const
{
    return OH_Drawing_FontGetTextSize(font_.get());
}

void Font::SetLinearText(bool isLinearText)
{
    OH_Drawing_FontSetLinearText(font_.get(), isLinearText);
}

bool Font::IsLinearText() const
{
    return OH_Drawing_FontIsLinearText(font_.get());
}

void Font::SetTextSkewX(float skewX)
{
    OH_Drawing_FontSetTextSkewX(font_.get(), skewX);
}

float Font::GetTextSkewX() const
{
    return OH_Drawing_FontGetTextSkewX(font_.get());
}

void Font::SetFakeBoldText(bool isFakeBoldText)
{
    OH_Drawing_FontSetFakeBoldText(font_.get(), isFakeBoldText);
}

bool Font::IsFakeBoldText() const
{
    return OH_Drawing_FontIsFakeBoldText(font_.get());
}

// 新增方法实现
void Font::SetScaleX(float scaleX)
{
    OH_Drawing_FontSetScaleX(font_.get(), scaleX);
}

float Font::GetScaleX() const
{
    return OH_Drawing_FontGetScaleX(font_.get());
}

void Font::SetHinting(FontHinting hinting)
{
    OH_Drawing_FontSetHinting(font_.get(), hinting);
}

Font::FontHinting Font::GetHinting() const
{
    return OH_Drawing_FontGetHinting(font_.get());
}

void Font::SetEmbeddedBitmaps(bool isEmbeddedBitmaps)
{
    OH_Drawing_FontSetEmbeddedBitmaps(font_.get(), isEmbeddedBitmaps);
}

bool Font::IsEmbeddedBitmaps() const
{
    return OH_Drawing_FontIsEmbeddedBitmaps(font_.get());
}

void Font::SetEdging(FontEdging edging)
{
    OH_Drawing_FontSetEdging(font_.get(), edging);
}

Font::FontEdging Font::GetEdging() const
{
    return OH_Drawing_FontGetEdging(font_.get());
}

void Font::SetBaselineSnap(bool baselineSnap)
{
    OH_Drawing_FontSetBaselineSnap(font_.get(), baselineSnap);
}

bool Font::IsBaselineSnap() const
{
    return OH_Drawing_FontIsBaselineSnap(font_.get());
}

void Font::SetSubpixel(bool isSubpixel)
{
    OH_Drawing_FontSetSubpixel(font_.get(), isSubpixel);
}

bool Font::IsSubpixel() const
{
    return OH_Drawing_FontIsSubpixel(font_.get());
}

void Font::SetForceAutoHinting(bool isForceAutoHinting)
{
    OH_Drawing_FontSetForceAutoHinting(font_.get(), isForceAutoHinting);
}

bool Font::IsForceAutoHinting() const
{
    return OH_Drawing_FontIsForceAutoHinting(font_.get());
}

// 文本测量与操作方法实现
int Font::CountText(const void *text, size_t byteLength, TextEncoding encoding)
{
    return OH_Drawing_FontCountText(font_.get(), text, byteLength, encoding);
}

uint32_t Font::TextToGlyphs(const void *text, uint32_t byteLength, TextEncoding encoding, uint16_t *glyphs,
                            int maxGlyphCount)
{
    return OH_Drawing_FontTextToGlyphs(font_.get(), text, byteLength, encoding, glyphs, maxGlyphCount);
}

void Font::GetWidths(const uint16_t *glyphs, int count, float *widths)
{
    OH_Drawing_FontGetWidths(font_.get(), glyphs, count, widths);
}

bool Font::MeasureSingleCharacter(const char *str, float *textWidth)
{
    return OH_Drawing_FontMeasureSingleCharacter(font_.get(), str, textWidth) == OH_DRAWING_SUCCESS;
}

bool Font::MeasureText(const void *text, size_t byteLength, TextEncoding encoding, OH_Drawing_Rect *bounds,
                       float *textWidth)
{
    return OH_Drawing_FontMeasureText(font_.get(), text, byteLength, encoding, bounds, textWidth) == OH_DRAWING_SUCCESS;
}

float Font::GetMetrics(FontMetrics *metrics)
{
    return OH_Drawing_FontGetMetrics(font_.get(), metrics);
}

} // namespace LottieTurbo::drawing