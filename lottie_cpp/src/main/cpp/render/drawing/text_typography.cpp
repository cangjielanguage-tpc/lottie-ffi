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

#include "text_typography.h"

namespace LottieTurbo::drawing {

// TypographyStyle 实现
TypographyStyle::TypographyStyle()
    : style_(UniqueNativeTypographyStyle(OH_Drawing_CreateTypographyStyle(), &OH_Drawing_DestroyTypographyStyle))
{
}

void TypographyStyle::SetTextDirection(TextDirection direction)
{
    OH_Drawing_SetTypographyTextDirection(style_.get(), static_cast<int>(direction));
}

void TypographyStyle::SetTextAlign(TextAlign align)
{
    OH_Drawing_SetTypographyTextAlign(style_.get(), static_cast<int>(align));
}

void TypographyStyle::SetMaxLines(int lineNumber)
{
    OH_Drawing_SetTypographyTextMaxLines(style_.get(), lineNumber);
}

// Typography 实现
Typography::Typography(OH_Drawing_Typography* typography)
    : typography_(UniqueNativeTypography(typography, &OH_Drawing_DestroyTypography))
{
}

void Typography::Layout(double maxWidth)
{
    OH_Drawing_TypographyLayout(typography_.get(), maxWidth);
}

void Typography::PaintOnPath(OH_Drawing_Canvas* canvas, const Path& path, double hOffset, double vOffset)
{
    OH_Drawing_TypographyPaintOnPath(typography_.get(), canvas, path.get(), hOffset, vOffset);
}

// TypographyHandler 实现
TypographyHandler::TypographyHandler(TypographyStyle& style, OH_Drawing_FontCollection* fontCollection)
    : handler_(UniqueNativeTypographyHandler(
        OH_Drawing_CreateTypographyHandler(style.get(), fontCollection),
        &OH_Drawing_DestroyTypographyHandler))
{
}

void TypographyHandler::PushTextStyle(OH_Drawing_TextStyle* style)
{
    OH_Drawing_TypographyHandlerPushTextStyle(handler_.get(), style);
}

void TypographyHandler::AddText(const std::string& text)
{
    OH_Drawing_TypographyHandlerAddText(handler_.get(), text.c_str());
}

void TypographyHandler::PopTextStyle()
{
    OH_Drawing_TypographyHandlerPopTextStyle(handler_.get());
}

Typography TypographyHandler::CreateTypography()
{
    return Typography(OH_Drawing_CreateTypography(handler_.get()));
}
// TextStyle 实现
TextStyle::TextStyle()
    : style_(UniqueNativeTextStyle(OH_Drawing_CreateTextStyle(), &OH_Drawing_DestroyTextStyle))
{
}

void TextStyle::SetColor(uint32_t color)
{
    OH_Drawing_SetTextStyleColor(style_.get(), color);
}

void TextStyle::SetFontFamilies(const std::string& fontFamily)
{
    std::vector<char> familyCopy(fontFamily.begin(), fontFamily.end());
    familyCopy.push_back('\0');
    const char* family = familyCopy.data();
    OH_Drawing_SetTextStyleFontFamilies(style_.get(), 1, &family);
}

void TextStyle::SetFontSize(double fontSize)
{
    OH_Drawing_SetTextStyleFontSize(style_.get(), fontSize);
}
} // namespace LottieTurbo::drawing