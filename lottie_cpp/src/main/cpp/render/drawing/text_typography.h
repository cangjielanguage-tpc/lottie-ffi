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

#ifndef LOTTIETURBO_TEXT_TYPOGRAPHY_H
#define LOTTIETURBO_TEXT_TYPOGRAPHY_H

#include <memory>
#include <string>
#include "path.h"
#include <native_drawing/drawing_text_typography.h>
#include <native_drawing/drawing_canvas.h>

namespace LottieTurbo::drawing {

// 封装Typography样式类
class TypographyStyle final {
    using UniqueNativeTypographyStyle = std::unique_ptr<OH_Drawing_TypographyStyle,
        decltype(&OH_Drawing_DestroyTypographyStyle)>;

public:
    using TextDirection = OH_Drawing_TextDirection;
    using TextAlign = OH_Drawing_TextAlign;

    TypographyStyle();
    ~TypographyStyle() = default;

    TypographyStyle(const TypographyStyle&) = delete;
    TypographyStyle& operator=(const TypographyStyle&) = delete;
    TypographyStyle(TypographyStyle&&) = default;
    TypographyStyle& operator=(TypographyStyle&&) = default;

    // 设置文本方向
    void SetTextDirection(TextDirection direction);

    // 设置文本对齐方式
    void SetTextAlign(TextAlign align);

    // 设置最大行数（与路径绘制相关的重要设置）
    void SetMaxLines(int lineNumber);

    OH_Drawing_TypographyStyle* get() const
    {
        return style_.get();
    }

private:
    UniqueNativeTypographyStyle style_;
};

// 封装Typography类
class Typography final {
    using UniqueNativeTypography = std::unique_ptr<OH_Drawing_Typography, decltype(&OH_Drawing_DestroyTypography)>;

public:
    explicit Typography(OH_Drawing_Typography* typography);
    ~Typography() = default;

    Typography(const Typography&) = delete;
    Typography& operator=(const Typography&) = delete;
    Typography(Typography&&) = default;
    Typography& operator=(Typography&&) = default;

    // 布局排版
    void Layout(double maxWidth);

    // 沿路径绘制文本
    void PaintOnPath(OH_Drawing_Canvas* canvas, const Path& path, double hOffset, double vOffset);

    OH_Drawing_Typography* get() const
    {
        return typography_.get();
    }

private:
    UniqueNativeTypography typography_;
};

// 封装Typography创建处理类
class TypographyHandler final {
    using UniqueNativeTypographyHandler = std::unique_ptr<OH_Drawing_TypographyCreate,
        decltype(&OH_Drawing_DestroyTypographyHandler)>;

public:
    TypographyHandler(TypographyStyle& style, OH_Drawing_FontCollection* fontCollection);
    ~TypographyHandler() = default;

    TypographyHandler(const TypographyHandler&) = delete;
    TypographyHandler& operator=(const TypographyHandler&) = delete;
    TypographyHandler(TypographyHandler&&) = default;
    TypographyHandler& operator=(TypographyHandler&&) = default;

    // 设置文本样式
    void PushTextStyle(OH_Drawing_TextStyle* style);

    // 添加文本内容
    void AddText(const std::string& text);

    // 移除顶部样式
    void PopTextStyle();

    // 创建Typography实例
    Typography CreateTypography();

    OH_Drawing_TypographyCreate* get() const
    {
        return handler_.get();
    }

private:
    UniqueNativeTypographyHandler handler_;
};
class TextStyle final {
public:
    // 添加构造函数
    TextStyle();

    // 添加析构函数（如果需要）
    ~TextStyle() = default;

    // 禁用拷贝构造和拷贝赋值（或者根据需要实现）
    TextStyle(const TextStyle&) = delete;
    TextStyle& operator=(const TextStyle&) = delete;

    // 启用移动构造和移动赋值
    TextStyle(TextStyle&&) = default;
    TextStyle& operator=(TextStyle&&) = default;

    // 设置颜色
    void SetColor(uint32_t color);

    // 设置字体族
    void SetFontFamilies(const std::string& fontFamily);

    // 获取原生文本样式指针
    OH_Drawing_TextStyle* get() const { return style_.get(); }

    // 设置字体大小
    void SetFontSize(double fontSize);

private:
    using UniqueNativeTextStyle = std::unique_ptr<OH_Drawing_TextStyle, decltype(&OH_Drawing_DestroyTextStyle)>;
    UniqueNativeTextStyle style_;
};
} // namespace LottieTurbo::drawing

#endif // LOTTIETURBO_TEXT_TYPOGRAPHY_H