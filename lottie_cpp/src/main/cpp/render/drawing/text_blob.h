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

#ifndef LOTTIETURBO_TEXT_BLOB_H
#define LOTTIETURBO_TEXT_BLOB_H

#include <memory>
#include <native_drawing/drawing_font.h>
#include <native_drawing/drawing_text_blob.h>
#include <string>

namespace LottieTurbo::drawing {

class TextBlob final {
    using UniqueNativeTextBlob = std::unique_ptr<OH_Drawing_TextBlob, decltype(&OH_Drawing_TextBlobDestroy)>;

public:
    // 创建用于访问控制的内部类
    class MakeUniqueEnabler {
    private:
        explicit MakeUniqueEnabler() = default;
        friend TextBlob;
    };

    // 公开但受控的构造函数（仅通过MakeUniqueEnabler访问）
    explicit TextBlob(OH_Drawing_TextBlob *textBlob, MakeUniqueEnabler);

    // 从文本创建TextBlob
    static std::unique_ptr<TextBlob> CreateFromText(const void *text, size_t byteLength, const OH_Drawing_Font *font,
                                                    OH_Drawing_TextEncoding textEncoding);

    // 从带位置的文本创建TextBlob
    static std::unique_ptr<TextBlob> CreateFromPosText(const void *text, size_t byteLength, OH_Drawing_Point2D *point2D,
                                                       const OH_Drawing_Font *font,
                                                       OH_Drawing_TextEncoding textEncoding);

    // 从字符串创建TextBlob
    static std::unique_ptr<TextBlob> CreateFromString(const char *str, const OH_Drawing_Font *font,
                                                      OH_Drawing_TextEncoding textEncoding);

    static std::unique_ptr<TextBlob> Create(OH_Drawing_TextBlob* blob)
    {
        if (blob == nullptr) {
            return nullptr;
        }
        // 使用std::make_unique并传递MakeUniqueEnabler标记
        return std::make_unique<TextBlob>(blob, MakeUniqueEnabler{});
    }

    // 获取边界
    void GetBounds(OH_Drawing_Rect *rect);

    // 获取唯一ID
    uint32_t GetUniqueID() const;

    OH_Drawing_TextBlob *get() const
    {
        return textBlob_.get();
    }
    friend std::unique_ptr<TextBlob> CreateTextBlobHelper(OH_Drawing_TextBlob* blob);
private:
    explicit TextBlob(OH_Drawing_TextBlob *textBlob);
    UniqueNativeTextBlob textBlob_;
    friend class TextBlobBuilder;
};

class TextBlobBuilder final {
    using UniqueNativeTextBlobBuilder =
        std::unique_ptr<OH_Drawing_TextBlobBuilder, decltype(&OH_Drawing_TextBlobBuilderDestroy)>;

public:
    TextBlobBuilder();

    // 分配运行空间
    const OH_Drawing_RunBuffer *AllocRunPos(const OH_Drawing_Font *font, int32_t count,
                                            const OH_Drawing_Rect *rect = nullptr);

    // 创建TextBlob
    std::unique_ptr<TextBlob> Make();

private:
    UniqueNativeTextBlobBuilder builder_;
};

} // namespace LottieTurbo::drawing

#endif