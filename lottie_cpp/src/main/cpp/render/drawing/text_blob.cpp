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

#include "text_blob.h"

namespace LottieTurbo::drawing {

TextBlob::TextBlob(OH_Drawing_TextBlob *textBlob) : textBlob_(textBlob, OH_Drawing_TextBlobDestroy)
{
}

// 受控的公开构造函数
TextBlob::TextBlob(OH_Drawing_TextBlob *textBlob, TextBlob::MakeUniqueEnabler)
    : textBlob_(textBlob, OH_Drawing_TextBlobDestroy)
{
}

std::unique_ptr<TextBlob> CreateTextBlobHelper(OH_Drawing_TextBlob* blob)
{
    return TextBlob::Create(blob);
}

std::unique_ptr<TextBlob> TextBlob::CreateFromText(const void *text, size_t byteLength, const OH_Drawing_Font *font,
                                                   OH_Drawing_TextEncoding textEncoding)
{
    auto blob = OH_Drawing_TextBlobCreateFromText(text, byteLength, font, textEncoding);
    if (blob == nullptr) {
        return nullptr;
    }
    return CreateTextBlobHelper(blob);
}

std::unique_ptr<TextBlob> TextBlob::CreateFromPosText(const void *text, size_t byteLength, OH_Drawing_Point2D *point2D,
                                                      const OH_Drawing_Font *font, OH_Drawing_TextEncoding textEncoding)
{
    auto blob = OH_Drawing_TextBlobCreateFromPosText(text, byteLength, point2D, font, textEncoding);
    if (blob == nullptr) {
        return nullptr;
    }
    return CreateTextBlobHelper(blob);
}

std::unique_ptr<TextBlob> TextBlob::CreateFromString(const char *str, const OH_Drawing_Font *font,
                                                     OH_Drawing_TextEncoding textEncoding)
{
    auto blob = OH_Drawing_TextBlobCreateFromString(str, font, textEncoding);
    if (blob == nullptr) {
        return nullptr;
    }
    return CreateTextBlobHelper(blob);
}

void TextBlob::GetBounds(OH_Drawing_Rect *rect)
{
    OH_Drawing_TextBlobGetBounds(textBlob_.get(), rect);
}

uint32_t TextBlob::GetUniqueID() const
{
    return OH_Drawing_TextBlobUniqueID(textBlob_.get());
}

TextBlobBuilder::TextBlobBuilder() : builder_(OH_Drawing_TextBlobBuilderCreate(), OH_Drawing_TextBlobBuilderDestroy)
{
}

const OH_Drawing_RunBuffer *TextBlobBuilder::AllocRunPos(const OH_Drawing_Font *font, int32_t count,
                                                         const OH_Drawing_Rect *rect)
{
    return OH_Drawing_TextBlobBuilderAllocRunPos(builder_.get(), font, count, rect);
}

std::unique_ptr<TextBlob> TextBlobBuilder::Make()
{
    auto blob = OH_Drawing_TextBlobBuilderMake(builder_.get());
    if (blob == nullptr) {
        return nullptr;
    }
    return CreateTextBlobHelper(blob);
}

} // namespace LottieTurbo::drawing