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

#include "render/lottie_render_node.h"
#include "bitmap.h"
#include "boundscheck/securec.h"
#include "brush.h"
#include "canvas.h"
#include "color.h"
#include "common/common.h"
#include "common/number_constant.h"
#include "font.h"
#include "font_collection.h"
#include "font_mgr.h"
#include "matrix.h"
#include "path.h"
#include "pen.h"
#include "point.h"
#include "rect.h"
#include "rlottiecommon.h"
#include "shader_effect.h"
#include "text_blob.h"
#include "text_typography.h"
#include <bits/alltypes.h>
#include <native_drawing/drawing_canvas.h>
#include <native_drawing/drawing_color_filter.h>
#include <native_drawing/drawing_text_typography.h>
#include <native_drawing/drawing_sampling_options.h>

namespace LottieTurbo {

LottieRenderNode::LottieRenderNode()
{
    brush_ = std::make_shared<drawing::Brush>();
    pen_ = std::make_shared<drawing::Pen>();
    path_ = std::make_shared<drawing::Path>();
    randomNum_ = random();
    currentLayerNode_ = nullptr;
}

LottieRenderNode::~LottieRenderNode()
{
}
void LottieRenderNode::SetCurrentTextLayerNode(const LOTLayerNode *layerNode)
{
    currentLayerNode_ = layerNode;
}

const LOTLayerNode *LottieRenderNode::GetCurrentTextLayerNode() const
{
    return currentLayerNode_;
}
void LottieRenderNode::ProcessNode(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node,
                                   std::shared_ptr<drawing::Path> rawClipMask)
{
    if (!node || !rawCanvas) {
        return;
    }
    // 检查节点是否需要渲染
    if (node->mFlag == ChangeFlagNone) {
        return;
    }

    // 处理文本节点
    if (node->mTextInfo.isTextNode && node->mTextInfo.text != nullptr) {
        ProcessTextNode(rawCanvas, node);
        renderInfo_.text++;
        return;
    }
    // 处理图像节点
    if (node->mImageInfo.data && node->mImageInfo.width > 0 && node->mImageInfo.height > 0) {
        ProcessImageNode(rawCanvas, node);
        renderInfo_.image++;
        return;
    }

    if (node->mPath.ptCount > 0) {
        ProcessPathNode(rawCanvas, node, rawClipMask);
        renderInfo_.path++;
        return;
    }
}

void LottieRenderNode::ProcessPathNode(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node,
                                       std::shared_ptr<drawing::Path> rawClipMask)
{
    path_->Reset();
    ConvertLottiePathToDrawingPath(node->mPath.ptPtr, node->mPath.ptCount, node->mPath.elmPtr, node->mPath.elmCount,
                                   *path_);

    // 应用裁剪蒙版
    bool clipMask =
        rawClipMask &&
        (!strokeProps_ ||
         strokeProps_->showType != DISPLAY_BACKGROUND); // 控制stroke效果的时候，showType为1的时候，需要绘制canvas背景
    if (clipMask) {
        rawCanvas->SaveLayer(nullptr, nullptr);
        renderInfo_.savelayer++;
        rawCanvas->ClipPath(rawClipMask->get(), INTERSECT, true);
        renderInfo_.mask++;
    }

    if (node->mType == 0) {
        SetupBrush(rawCanvas, node);
    }

    if (node->mStroke.enable && node->mStroke.width > 0) {
        SetupPen(rawCanvas, node);
    } else if (node->mType != 0) {
        brush_->Reset();
        brush_->SetColor(0);
        rawCanvas->AttachBrush(*brush_);
    }

    // 绘制路径
    if (isRenderStroke()) {
        StrokeEffect(rawCanvas, rawClipMask);
    } else {
        rawCanvas->DrawPath(path_->get());
    }

    // 解除画笔和画刷绑定
    rawCanvas->DetachBrush();
    if (isDetachPen_) {
        rawCanvas->DetachPen();
    }
    // 恢复状态
    if (clipMask) {
        rawCanvas->Restore();
    }
}

std::unique_ptr<drawing::Font> LottieRenderNode::InitializeFont(const LOTNode *node)
{
    auto font = std::make_unique<drawing::Font>();
    if (!font) {
        DRAWING_LOGE("LottieRenderNode::InitializeFont: Failed to create font");
        return nullptr;
    }

    font->SetTextSize(node->mTextInfo.fontSize);

    // 尝试设置指定的字体,如果失败会自动使用默认字体
    if (node->mTextInfo.fontName) {
        drawing::FontMgr fontMgr;
        auto styleSet = fontMgr.MatchFamily(node->mTextInfo.fontName);
        if (styleSet) {
            auto typeface = styleSet->CreateTypeface(0);
            if (typeface) {
                font->SetTypeface(typeface);
            } else {
                DRAWING_LOGD("LottieRenderNode::InitializeFont: Failed to load font: %{public}s",
                             node->mTextInfo.fontName);
            }
        }
    }

    return font;
}

std::vector<std::string> LottieRenderNode::SplitTextIntoLines(const char *text)
{
    std::string textStr(text ? text : " ");
    std::vector<std::string> lines;
    size_t start = 0;
    size_t end = 0;

    while ((end = textStr.find("\r", start)) != std::string::npos) {
        lines.push_back(textStr.substr(start, end - start));
        start = end + 1;
    }

    lines.push_back(textStr.substr(start));
    return lines;
}

void LottieRenderNode::ApplyTextStroke(std::shared_ptr<drawing::Canvas> rawCanvas, const drawing::TextBlob *textBlob,
                                       const TextPosition &position, const LOTNode *node)
{
    if (!node->mTextInfo.hasStroke ||
        !((node->mTextInfo.sr > 0 || node->mTextInfo.sg > 0 || node->mTextInfo.sb > 0) && node->mTextInfo.sa > 0) ||
        node->mTextInfo.strokeWidth <= 0) {
        return;
    }

    // 保存当前画布状态
    rawCanvas->Save();

    // 设置描边样式
    pen_->Reset();
    uint32_t strokeColor =
        drawing::Color::SetArgb(node->mTextInfo.sa, node->mTextInfo.sr, node->mTextInfo.sg, node->mTextInfo.sb);
    pen_->SetColor(strokeColor);
    pen_->SetWidth(node->mTextInfo.strokeWidth);
    pen_->SetAntiAlias(true);

    // 应用描边画笔
    rawCanvas->AttachPen(*pen_);

    // 绘制描边文本
    rawCanvas->DrawTextBlob(textBlob->get(), position.x, position.y);

    // 恢复画布状态
    rawCanvas->DetachPen();
    rawCanvas->Restore();
}

void LottieRenderNode::DrawTextBlob(std::shared_ptr<drawing::Canvas> rawCanvas, drawing::Font *font,
                                    const std::string &line, const TextDrawParams &params, const LOTNode *node)
{
    if (line.empty()) {
        return;
    }

    // 创建textBlob
    auto textBlob =
        drawing::TextBlob::CreateFromString(line.c_str(), font->get(), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    if (!textBlob) {
        DRAWING_LOGD("LottieRenderNode::DrawTextBlob: Failed to create text blob");
        return;
    }

    // 绘制文本和描边
    rawCanvas->DrawTextBlob(textBlob->get(), params.position.x, params.position.y);
    ApplyTextStroke(rawCanvas, textBlob.get(), params.position, node);
}

// 提取单个UTF-8字符
int LottieRenderNode::ExtractUtf8Char(const std::string &line, size_t index, char *outChar, size_t outCharSize)
{
    // 处理UTF-8多字节字符
    unsigned char firstByte = static_cast<unsigned char>(line[index]);
    int charBytes = 0;

    if (firstByte < 0x80) { // ASCII
        charBytes = 1;
    } else if ((firstByte & 0xE0) == 0xC0) { // 2字节
        charBytes = 2;
    } else if ((firstByte & 0xF0) == 0xE0) { // 3字节
        charBytes = 3;
    } else if ((firstByte & 0xF8) == 0xF0) { // 4字节
        charBytes = 4;
    } else {
        // 非法UTF-8序列
        return 0;
    }

    // 确保不会越界
    if (index + charBytes > line.length()) {
        return 0;
    }

    // 复制字符
    auto errorNum = strncpy_s(outChar, outCharSize, line.c_str() + index, charBytes);
    if (errorNum != EOK) {
        return 0;
    }
    outChar[charBytes] = '\0';

    return charBytes;
}

// 绘制单个字符并计算下一个位置
void LottieRenderNode::DrawSingleChar(std::shared_ptr<drawing::Canvas> rawCanvas, drawing::Font *font,
                                      const CharDrawParams &params, const LOTNode *node, float &outNextX)
{
    // 创建单字符textBlob
    auto charBlob = drawing::TextBlob::CreateFromString(params.singleChar, font->get(),
                                                        OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
    if (!charBlob) {
        outNextX = params.position.x;
        return;
    }

    // 绘制字符
    TextPosition charPos = {params.position.x, params.position.y};
    rawCanvas->DrawTextBlob(charBlob->get(), params.position.x, params.position.y);
    ApplyTextStroke(rawCanvas, charBlob.get(), charPos, node);

    // 计算字符宽度并更新位置
    float charWidth = 0;
    font->MeasureText(params.singleChar, params.charBytes, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, nullptr,
                      &charWidth);
    outNextX = params.position.x + charWidth + params.tracking;
}

void LottieRenderNode::DrawCharByChar(std::shared_ptr<drawing::Canvas> rawCanvas, drawing::Font *font,
                                      const std::string &line, const TextDrawParams &params, const LOTNode *node)
{
    if (line.empty()) {
        return;
    }

    float currentX = params.position.x;

    for (size_t i = 0; i < line.length();) {
        // 提取单个字符
        char singleChar[5] = {0}; // UTF-8最多4字节 + 结束符
        int charBytes = ExtractUtf8Char(line, i, singleChar, sizeof(singleChar));
        if (charBytes <= 0) {
            i++;
            continue;
        }
        // 准备绘制参数
        CharDrawParams drawParams = {{currentX, params.position.y}, params.tracking, singleChar, charBytes};
        // 绘制字符并获取下一个位置
        float nextX;
        DrawSingleChar(rawCanvas, font, drawParams, node, nextX);
        currentX = nextX;

        // 移到下一个字符
        i += charBytes;
    }
}

void LottieRenderNode::DrawSingleLine(std::shared_ptr<drawing::Canvas> rawCanvas, drawing::Font *font,
                                      const std::string &line, const TextDrawParams &params, const LOTNode *node)
{
    if (params.tracking == 0.0f) {
        DrawTextBlob(rawCanvas, font, line, params, node);
    } else {
        DrawCharByChar(rawCanvas, font, line, params, node);
    }
}

int LottieRenderNode::GetUTF8CharBytes(char firstByte)
{
    unsigned char byte = static_cast<unsigned char>(firstByte);
    if (byte < 0x80)
        return NUM_1;
    if ((byte & 0xE0) == 0xC0)
        return NUM_2;
    if ((byte & 0xF0) == 0xE0)
        return NUM_3;
    if ((byte & 0xF8) == 0xF0)
        return NUM_4;
    return 1; // 默认情况
}

size_t LottieRenderNode::FindBreakPoint(drawing::Font *font, const std::string &line, float maxWidth, float tracking)
{
    if (line.empty()) {
        return 0;
    }

    // 缩放tracking值，使其更合理
    const float TRACKING_SCALE_FACTOR = 0.01f;
    float scaledTracking = tracking * TRACKING_SCALE_FACTOR;

    float currentWidth = 0.0f;
    size_t lastBreakPoint = 0;

    // 逐字符测量宽度
    for (size_t i = 0; i < line.length();) {
        // 获取单个UTF-8字符
        int charBytes = GetUTF8CharBytes(line[i]);
        if (i + charBytes > line.length()) {
            break;
        }

        // 提取当前字符
        std::string currentChar = line.substr(i, charBytes);

        // 测量字符宽度
        float charWidth = 0.0f;
        font->MeasureText(currentChar.c_str(), charBytes, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, nullptr,
                          &charWidth);

        // 如果添加这个字符会超出宽度，返回上一个断点
        if (currentWidth + charWidth + (i > 0 ? scaledTracking : 0) > maxWidth) {
            return lastBreakPoint > 0 ? lastBreakPoint : (i > 0 ? i : charBytes);
        }

        // 更新当前宽度
        currentWidth += charWidth;
        if (i > 0) { // 不是第一个字符时，添加字符间距
            currentWidth += scaledTracking;
        }

        // 记录当前位置作为可能的断点
        lastBreakPoint = i + charBytes;

        // 移动到下一个字符
        i += charBytes;
    }

    // 如果整行都可以放下，返回行的长度
    return line.length();
}

// 初始化并设置文本处理配置
TextProcessConfig LottieRenderNode::InitializeTextProcessing(std::shared_ptr<drawing::Canvas> rawCanvas,
                                                             const LOTNode *node, std::unique_ptr<drawing::Font> &font)
{
    // 初始化字体
    font = InitializeFont(node);
    if (!font) {
        return {};
    }

    drawing::Matrix matrix;
    matrix.SetMatrix({node->mTextInfo.mTextMatrix.m11, node->mTextInfo.mTextMatrix.m21, node->mTextInfo.mTextMatrix.m31,
                      node->mTextInfo.mTextMatrix.m12, node->mTextInfo.mTextMatrix.m22, node->mTextInfo.mTextMatrix.m32,
                      node->mTextInfo.mTextMatrix.m13, node->mTextInfo.mTextMatrix.m23,
                      node->mTextInfo.mTextMatrix.m33});
    rawCanvas->ConcatMatrix(matrix.get());

    // 构建配置结构
    TextProcessConfig config;
    config.textBoxWidth = node->mTextInfo.textBoxWidth;
    config.textBoxHeight = node->mTextInfo.textBoxHeight;
    config.textBoxPosX = node->mTextInfo.textBoxPosX;
    config.textBoxPosY = node->mTextInfo.textBoxPosY;
    config.hasTextBox = (config.textBoxWidth > 0 && config.textBoxHeight > 0);
    config.lineHeight = node->mTextInfo.lineHeight;
    config.textJustify = static_cast<int>(node->mTextInfo.textJustify);
    config.baselineShift = node->mTextInfo.baselineShift;
    config.tracking = node->mTextInfo.tracking;

    // 缩放tracking值以获得合理的字距
    const float TRACKING_SCALE_FACTOR = 0.01f;
    config.scaledTracking = config.tracking * TRACKING_SCALE_FACTOR;

    // 应用文本框位置
    if (config.textBoxPosX != 0 || config.textBoxPosY != 0) {
        rawCanvas->Translate(config.textBoxPosX, config.textBoxPosY);
    }

    return config;
}

// 处理文本换行
std::vector<std::string> LottieRenderNode::ProcessTextWrapping(const LOTNode *node, drawing::Font *font,
                                                               const TextProcessConfig &config)
{
    // 分割文本为多行（按\r分割）
    std::vector<std::string> originalLines = SplitTextIntoLines(node->mTextInfo.text);
    std::vector<std::string> processedLines;

    if (!config.hasTextBox && config.textBoxWidth <= 0) {
        processedLines = originalLines;
        return processedLines;
    }

    for (const auto &line : originalLines) {
        if (line.empty()) {
            processedLines.push_back(line);
            continue;
        }

        // 测量当前行宽度
        float lineWidth = 0.0f;
        font->MeasureText(line.c_str(), line.length(), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, nullptr,
                          &lineWidth);

        // 如果有字距调整，添加额外宽度
        if (config.tracking != 0) {
            size_t charCount = 0;
            for (size_t i = 0; i < line.length();) {
                int charBytes = GetUTF8CharBytes(line[i]);
                i += charBytes;
                charCount++;
            }
            if (charCount > 1) {
                lineWidth += config.scaledTracking * (charCount - 1);
            }
        }

        // 如果行宽度不超过文本框宽度，直接添加
        if (lineWidth <= config.textBoxWidth) {
            processedLines.push_back(line);
            continue;
        }

        // 需要自动换行
        std::string remainingText = line;
        while (!remainingText.empty()) {
            size_t breakPoint = FindBreakPoint(font, remainingText, config.textBoxWidth, config.tracking);
            if (breakPoint == 0) {
                // 至少包含一个字符
                breakPoint = GetUTF8CharBytes(remainingText[0]);
            }
            processedLines.push_back(remainingText.substr(0, breakPoint));
            remainingText = remainingText.substr(breakPoint);
        }
    }

    return processedLines;
}

// 执行文本渲染
void LottieRenderNode::ExecuteTextRendering(std::shared_ptr<drawing::Canvas> rawCanvas, drawing::Font *font,
                                            const std::vector<std::string> &processedLines, const LOTNode *node,
                                            const TextProcessConfig &config)
{
    // 构建渲染参数
    TextRenderParams renderParams = {config.lineHeight,     config.textJustify, config.baselineShift,
                                     config.scaledTracking, config.hasTextBox,  config.textBoxWidth};

    // 根据是否有文本范围选择不同的渲染方式
    if (node->mTextInfo.hasTextRange) {
        ProcessTextWithRange(rawCanvas, font, processedLines, node, renderParams);
    } else {
        ProcessTextWithoutRange(rawCanvas, font, processedLines, node, renderParams);
    }

    // 清理资源
    rawCanvas->DetachBrush();
}

bool LottieRenderNode::ProcessTextPath(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node,
                                       const LOTLayerNode *layerNode)
{
    // 验证输入参数和准备路径
    auto pathObj = ValidateAndPreparePath(node, layerNode);
    if (!pathObj) {
        return false;
    }

    // 创建Typography对象
    auto typography = CreateTextPathTypographyObject(node);
    if (!typography.get()) {
        return false;
    }

    // 执行文本路径渲染
    return RenderTextOnPath(rawCanvas, node, pathObj, typography);
}

std::shared_ptr<drawing::Path> LottieRenderNode::ValidateAndPreparePath(const LOTNode *node,
                                                                        const LOTLayerNode *layerNode)
{
    // 基础验证
    if (!node->mTextInfo.hasTextPath || !node || !layerNode) {
        return nullptr;
    }

    // 验证蒙版数据
    if (!layerNode->mMaskList.ptr || layerNode->mMaskList.size <= static_cast<size_t>(node->mTextInfo.maskIndex) ||
        node->mTextInfo.maskIndex < 0) {
        return nullptr;
    }

    const LOTMask *mask = &layerNode->mMaskList.ptr[node->mTextInfo.maskIndex];

    // 确保蒙版有有效路径
    if (!mask || mask->mPath.ptCount == 0) {
        return nullptr;
    }

    // 创建并转换路径
    auto pathObj = std::make_shared<drawing::Path>();
    ConvertLottiePathToDrawingPath(mask->mPath.ptPtr, mask->mPath.ptCount, mask->mPath.elmPtr, mask->mPath.elmCount,
                                   *pathObj);

    // 处理路径反转
    if (node->mTextInfo.reversePath) {
        DRAWING_LOGE("LottieRenderNode::ValidateAndPreparePath: Path reversal not implemented");
    }

    return pathObj;
}

drawing::Typography LottieRenderNode::CreateTextPathTypographyObject(const LOTNode *node)
{
    // 创建FontCollection
    drawing::FontCollection fontCollection;
    if (!fontCollection.get()) {
        fontCollection = drawing::FontCollection::CreateShared();
    }
    if (!fontCollection.get()) {
        return drawing::Typography(nullptr);
    }

    // 配置Typography样式
    drawing::TypographyStyle typographyStyle;
    typographyStyle.SetTextAlign(drawing::TypographyStyle::TextAlign::TEXT_ALIGN_LEFT);
    typographyStyle.SetTextDirection(drawing::TypographyStyle::TextDirection::TEXT_DIRECTION_LTR);
    typographyStyle.SetMaxLines(1);

    // 强制对齐处理
    if (node->mTextInfo.forceAlignment) {
        typographyStyle.SetTextAlign(drawing::TypographyStyle::TextAlign::TEXT_ALIGN_CENTER);
    }

    // 创建Typography对象
    auto typography = CreateTextPathTypography(node, fontCollection, typographyStyle);
    if (typography.get()) {
        float layoutWidth = CalculateTypographyLayoutWidth(node);
        typography.Layout(layoutWidth);
    }

    return typography;
}

float LottieRenderNode::CalculateTypographyLayoutWidth(const LOTNode *node)
{
    if (!node || !node->mTextInfo.text) {
        return 1000.0f; // 默认值
    }

    // 创建临时字体对象来测量文本
    auto tempFont = InitializeFont(node);
    if (!tempFont) {
        return 1000.0f; // 默认值
    }

    // 测量文本实际宽度
    float textWidth = 0.0f;
    std::string text(node->mTextInfo.text);
    tempFont->MeasureText(text.c_str(), text.length(), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, nullptr,
                          &textWidth);

    // 考虑字距调整
    if (node->mTextInfo.tracking != 0) {
        const float TRACKING_SCALE_FACTOR = 0.01f;
        float scaledTracking = node->mTextInfo.tracking * TRACKING_SCALE_FACTOR;

        // 计算字符数量
        size_t charCount = 0;
        for (size_t i = 0; i < text.length();) {
            i += GetUTF8CharBytes(text[i]);
            charCount++;
        }
        if (charCount > 1) {
            textWidth += scaledTracking * (charCount - 1);
        }
    }

    // 考虑边距和偏移量
    float layoutWidth = textWidth;

    // 考虑首尾边距
    layoutWidth += node->mTextInfo.firstMargin;
    layoutWidth += node->mTextInfo.lastMargin;

    return layoutWidth;
}

bool LottieRenderNode::RenderTextOnPath(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node,
                                        std::shared_ptr<drawing::Path> pathObj, drawing::Typography &typography)
{
    if (!rawCanvas || !pathObj || !typography.get()) {
        return false;
    }

    // 计算路径偏移
    TextPathOffset offset(node->mTextInfo.firstMargin, node->mTextInfo.baselineShift);

    // 保存画布状态
    rawCanvas->Save();

    // 设置画刷并绘制文本
    brush_->Reset();
    uint32_t textColor =
        drawing::Color::SetArgb(node->mTextInfo.a, node->mTextInfo.r, node->mTextInfo.g, node->mTextInfo.b);
    brush_->SetColor(textColor);
    rawCanvas->AttachBrush(*brush_);

    // 沿路径绘制文本
    typography.PaintOnPath(rawCanvas->get(), *pathObj, offset.horizontal, offset.vertical);

    // 处理描边
    RenderTextPathStroke(rawCanvas, node, pathObj, typography, offset);

    // 清理并返回
    rawCanvas->DetachBrush();
    rawCanvas->Restore();
    return true;
}

void LottieRenderNode::RenderTextPathStroke(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node,
                                            std::shared_ptr<drawing::Path> pathObj, drawing::Typography &typography,
                                            const TextPathOffset &offset)
{
    // 检查是否需要描边
    if (!node->mTextInfo.hasStroke || node->mTextInfo.strokeWidth <= 0 ||
        !((node->mTextInfo.sr > 0 || node->mTextInfo.sg > 0 || node->mTextInfo.sb > 0) && node->mTextInfo.sa > 0)) {
        return;
    }

    // 配置描边
    pen_->Reset();
    uint32_t strokeColor =
        drawing::Color::SetArgb(node->mTextInfo.sa, node->mTextInfo.sr, node->mTextInfo.sg, node->mTextInfo.sb);
    pen_->SetColor(strokeColor);
    pen_->SetWidth(node->mTextInfo.strokeWidth);
    pen_->SetAntiAlias(true);

    // 绘制描边
    rawCanvas->AttachPen(*pen_);
    typography.PaintOnPath(rawCanvas->get(), *pathObj, offset.horizontal, offset.vertical);
    rawCanvas->DetachPen();
}

drawing::Typography LottieRenderNode::CreateTextPathTypography(const LOTNode *node,
                                                               drawing::FontCollection &fontCollection,
                                                               drawing::TypographyStyle &typographyStyle)
{
    // === 创建Typography处理器 ===
    drawing::TypographyHandler handler(typographyStyle, fontCollection.get());

    // === 配置文本样式 ===
    drawing::TextStyle textStyle;
    textStyle.SetFontSize(node->mTextInfo.fontSize);

    uint32_t textColor =
        drawing::Color::SetArgb(node->mTextInfo.a, node->mTextInfo.r, node->mTextInfo.g, node->mTextInfo.b);
    textStyle.SetColor(textColor);

    // 设置字体族
    if (node->mTextInfo.fontName) {
        textStyle.SetFontFamilies(std::string(node->mTextInfo.fontName));
    }

    // === 构建Typography ===
    handler.PushTextStyle(textStyle.get());
    handler.AddText(node->mTextInfo.text ? node->mTextInfo.text : "");
    handler.PopTextStyle();

    return handler.CreateTypography();
}

// 处理文本节点
void LottieRenderNode::ProcessTextNode(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node)
{
    if (!rawCanvas || !node) {
        return;
    }

    // 检查是否需要处理文本路径
    if (node->mTextInfo.hasTextPath) {
        const LOTLayerNode *layerNode = GetCurrentTextLayerNode();
        if (ProcessTextPath(rawCanvas, node, layerNode)) {
            return;
        }
    }

    std::unique_ptr<drawing::Font> font;

    // 初始化文本处理配置
    TextProcessConfig config = InitializeTextProcessing(rawCanvas, node, font);
    if (!font) {
        return;
    }

    // 处理文本换行
    std::vector<std::string> processedLines = ProcessTextWrapping(node, font.get(), config);

    // 执行文本渲染
    ExecuteTextRendering(rawCanvas, font.get(), processedLines, node, config);
}

// 计算文本范围参数
TextRangeParams LottieRenderNode::CalculateTextRangeParams(const LOTNode *node, size_t charIndex,
                                                           const std::vector<std::string> &processedLines)
{
    TextRangeParams params;
    params.start = node->mTextInfo.textRangeStart;
    params.end = node->mTextInfo.textRangeEnd;
    params.offset = node->mTextInfo.offset;

    // 应用普通偏移
    params.start += params.offset;
    params.end += params.offset;

    // 实现 basedOn 字段功能
    auto [effectiveIndex, effectiveTotal] = CalculateEffectiveIndex(node, charIndex, processedLines);
    params.effectiveIndex = effectiveIndex;
    params.effectiveTotal = effectiveTotal;

    // 转换为有效索引
    if (node->mTextInfo.textRangeUnit == 1) { // 百分比模式
        params.start = params.start / 100.0f * effectiveTotal;
        params.end = params.end / 100.0f * effectiveTotal;
    }

    params.hasRandomize = (node->mTextInfo.randomize > 0);
    params.randomize = node->mTextInfo.randomize;

    return params;
}

// 应用形状因子和其他效果
float LottieRenderNode::ApplyShapeAndEffects(const LOTNode *node, float relativePosition)
{
    // 应用形状函数
    float shapeFactor = CalculateShapeFactor(relativePosition, node->mTextInfo.shape);

    // 应用最大量属性
    shapeFactor *= (node->mTextInfo.maxAmount / 100.0f);

    // 应用缓动范围
    if (node->mTextInfo.minEase > 0 || node->mTextInfo.maxEase > 0) {
        float minEase = node->mTextInfo.minEase / 100.0f;
        float maxEase = node->mTextInfo.maxEase / 100.0f;
        shapeFactor = minEase + shapeFactor * (maxEase - minEase);
    }

    // 应用选择器平滑度
    if (node->mTextInfo.selectorSmoothness < 100.0f) {
        float smoothness = node->mTextInfo.selectorSmoothness / 100.0f;
        float edgeDistance = std::min(relativePosition, 1.0f - relativePosition) * 2.0f;
        float smoothFactor = 1.0f - (1.0f - smoothness) * (1.0f - edgeDistance);
        shapeFactor *= smoothFactor;
    }

    return std::max(0.0f, std::min(1.0f, shapeFactor));
}

TextRangeInfo LottieRenderNode::CalculateTextRangeInfo(const LOTNode *node, size_t charIndex,
                                                       const std::vector<std::string> &processedLines)
{
    TextRangeInfo info = {false, 0.0f};

    if (!node->mTextInfo.hasTextRange) {
        info.isInRange = true;
        info.factor = 1.0f;
        return info;
    }

    // 计算文本范围参数
    TextRangeParams params = CalculateTextRangeParams(node, charIndex, processedLines);

    // 随机化逻辑
    if (params.hasRandomize) {
        // 计算原始范围内应该选择的字符数
        size_t originalRangeSize = static_cast<size_t>(std::max(0.0f, params.end - params.start + 1));

        // 随机选择字符，但保持总数不变
        bool isSelected =
            IsCharacterRandomlySelected(node, params.effectiveIndex, params.effectiveTotal, originalRangeSize);
        if (isSelected) {
            info.isInRange = true;
            // 计算该字符在原始范围中的相对位置
            float relativePosition =
                CalculateRelativePosition(params.effectiveIndex, params.start, params.end, params.effectiveTotal);
            // 应用形状因子和其他效果
            info.factor = ApplyShapeAndEffects(node, relativePosition);
        }
    } else {
        // 无随机化：正常范围检查
        bool inBaseRange = (params.effectiveIndex >= params.start && params.effectiveIndex <= params.end);

        if (inBaseRange) {
            info.isInRange = true;

            float position = 0.0f;
            if (params.end > params.start) {
                position = (params.effectiveIndex - params.start) / (params.end - params.start);
            }

            // 应用形状因子和其他效果
            info.factor = ApplyShapeAndEffects(node, position);
        }
    }

    return info;
}

bool LottieRenderNode::IsCharacterRandomlySelected(const LOTNode *node, size_t charIndex, size_t totalChars,
                                                   size_t targetCount)
{
    if (node->mTextInfo.randomize == 0 || targetCount == 0) {
        return false;
    }
    std::mt19937 generator(randomNum_);
    std::vector<size_t> allIndices;
    for (size_t i = 0; i < totalChars; i++) {
        allIndices.push_back(i);
    }
    std::shuffle(allIndices.begin(), allIndices.end(), generator);

    // 避免类型歧义的简单检查
    size_t selectionCount = targetCount;
    if (selectionCount > totalChars) {
        selectionCount = totalChars;
    }
    // 检查当前字符是否在前 selectionCount 个选中的索引中
    for (size_t i = 0; i < selectionCount; i++) {
        if (allIndices[i] == charIndex) {
            return true;
        }
    }
    return false;
}

float LottieRenderNode::CalculateRelativePosition(size_t charIndex, float start, float end, size_t totalChars)
{
    // 计算字符在原始范围中的相对位置（0.0 - 1.0）
    if (end <= start) {
        return 0.0f;
    }
    // 将字符索引映射到原始范围中的位置
    float normalizedIndex = static_cast<float>(charIndex) / static_cast<float>(totalChars);
    float normalizedStart = start / static_cast<float>(totalChars);
    float normalizedEnd = end / static_cast<float>(totalChars);

    if (normalizedEnd <= normalizedStart) {
        return 0.0f;
    }
    // 计算相对位置
    float relativePos = (normalizedIndex - normalizedStart) / (normalizedEnd - normalizedStart);

    // 确保在有效范围内
    return std::max(0.0f, std::min(1.0f, relativePos));
}

float LottieRenderNode::CalculateShapeFactor(float position, int shape)
{
    // position 范围是 0.0 - 1.0
    switch (shape) {
        case NUM_1: // Square - 矩形波，所有位置都是最大值
            return 1.0f;

        case NUM_2: // Ramp Up - 线性上升
            return position;

        case NUM_3: // Ramp Down - 线性下降
            return 1.0f - position;

        case NUM_4: // Triangle - 三角波
            return position <= 0.5f ? position * 2.0f : (1.0f - position) * 2.0f;

        case NUM_5: // Round - 正弦波的一半
            return (1.0f - std::cos(position * M_PI)) * 0.5f;

        case NUM_6: // Smooth - 平滑曲线 (smoothstep)
            return position * position * (3.0f - 2.0f * position);

        default:
            return 1.0f;
    }
}

void LottieRenderNode::CalculateTextLineLayout(drawing::Font *font, const std::string &line,
                                               const TextRenderParams &params, float &startX, float &textWidth)
{
    // 计算文本宽度
    font->MeasureText(line.c_str(), line.length(), OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, nullptr, &textWidth);

    if (params.scaledTracking != 0) {
        size_t charCount = 0;
        for (size_t i = 0; i < line.length();) {
            i += GetUTF8CharBytes(line[i]);
            charCount++;
        }
        if (charCount > 1) {
            textWidth += params.scaledTracking * (charCount - 1);
        }
    }

    // 根据对齐方式计算起始X坐标
    startX = 0;
    switch (params.textJustify) {
        case NUM_1: // 右对齐
            startX = params.hasTextBox ? params.textBoxWidth - textWidth : (0 - textWidth);
            break;
        case NUM_2: // 居中对齐
            startX = params.hasTextBox ? (params.textBoxWidth - textWidth) / NUM_2 : (0 - textWidth) / NUM_2;
            break;
        case NUM_0: // 左对齐
        default:
            startX = 0;
            break;
    }
}

void LottieRenderNode::RenderCharWithRangeEffect(std::shared_ptr<drawing::Canvas> rawCanvas, drawing::Font *font,
                                                 const LOTNode *node, const CharPositionInfo &charInfo)
{
    int charBytes = GetUTF8CharBytes(charInfo.singleChar[0]);

    // 保存当前画布状态
    rawCanvas->Save();

    if (charInfo.rangeInfo.isInRange && charInfo.rangeInfo.factor > 0.0f) {
        // 范围内的字符：应用文本范围效果
        ApplyTextRangeTransform(rawCanvas, node, charInfo.rangeInfo.factor, charInfo.currentX, charInfo.y);
        SetTextRangeColor(rawCanvas, node, charInfo.rangeInfo.factor);

        auto charBlob = drawing::TextBlob::CreateFromString(charInfo.singleChar.c_str(), font->get(),
                                                            OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);

        if (charBlob) {
            rawCanvas->DrawTextBlob(charBlob->get(), 0, 0);
            ApplyTextRangeStroke(rawCanvas, charBlob.get(), {0, 0}, node, charInfo.rangeInfo.factor);
        }
    } else {
        // 范围外的字符：使用原始样式渲染
        rawCanvas->Translate(charInfo.currentX, charInfo.y);
        brush_->Reset();
        uint32_t textColor =
            drawing::Color::SetArgb(node->mTextInfo.a, node->mTextInfo.r, node->mTextInfo.g, node->mTextInfo.b);
        brush_->SetColor(textColor);
        rawCanvas->AttachBrush(*brush_);

        auto charBlob = drawing::TextBlob::CreateFromString(charInfo.singleChar.c_str(), font->get(),
                                                            OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8);
        if (charBlob != nullptr) {
            rawCanvas->DrawTextBlob(charBlob->get(), 0, 0);
            ApplyTextStroke(rawCanvas, charBlob.get(), {0, 0}, node);
        }
    }
    rawCanvas->Restore();
    // 计算字符宽度并更新位置
    float charWidth = 0;
    font->MeasureText(charInfo.singleChar.c_str(), charBytes, OH_Drawing_TextEncoding::TEXT_ENCODING_UTF8, nullptr,
                      &charWidth);
    charInfo.currentX += charWidth + charInfo.scaledTracking;
}

void LottieRenderNode::ProcessTextWithRange(std::shared_ptr<drawing::Canvas> rawCanvas, drawing::Font *font,
                                            const std::vector<std::string> &processedLines, const LOTNode *node,
                                            const TextRenderParams &params)
{
    float y = 0 + params.baselineShift;
    size_t globalCharIndex = 0;

    for (const auto &line : processedLines) {
        if (line.empty()) {
            y += params.lineHeight;
            continue;
        }

        // 计算行对齐
        float textWidth = 0;
        float startX = 0;
        CalculateTextLineLayout(font, line, params, startX, textWidth);
        float currentX = startX;

        // 逐字符处理
        for (size_t i = 0; i < line.length();) {
            // 获取单个字符
            int charBytes = GetUTF8CharBytes(line[i]);
            if (i + charBytes > line.length()) {
                break;
            }

            std::string singleChar = line.substr(i, charBytes);

            // 计算该字符的范围信息
            TextRangeInfo rangeInfo = CalculateTextRangeInfo(node, globalCharIndex, processedLines);

            // 渲染单个字符
            RenderCharWithRangeEffect(rawCanvas, font, node,
                                      {singleChar, rangeInfo, currentX, y, params.scaledTracking});

            i += charBytes;
            globalCharIndex++;
        }

        y += params.lineHeight;
    }
}

void LottieRenderNode::ProcessTextWithoutRange(std::shared_ptr<drawing::Canvas> rawCanvas, drawing::Font *font,
                                               const std::vector<std::string> &processedLines, const LOTNode *node,
                                               const TextRenderParams &params)
{
    float y = 0 + params.baselineShift;
    for (const auto &line : processedLines) {
        if (line.empty()) {
            y += params.lineHeight;
            continue;
        }

        // 计算行对齐
        float textWidth = 0;
        float startX = 0;
        CalculateTextLineLayout(font, line, params, startX, textWidth);

        // 绘制单行文本
        DrawSingleLine(rawCanvas, font, line, {params.scaledTracking, {startX, y}}, node);
        y += params.lineHeight;
    }
}

void LottieRenderNode::ApplyTextRangeTransform(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node,
                                               float factor, float x, float y)
{
    // 移动到字符位置
    rawCanvas->Translate(x, y);

    // 应用范围内的锚点偏移（先应用锚点）
    if (node->mTextInfo.hasRangeAnchor) {
        rawCanvas->Translate(-node->mTextInfo.rangeAnchorX * factor, -node->mTextInfo.rangeAnchorY * factor);
    }

    // 应用范围内的旋转
    if (node->mTextInfo.hasRangeRotation) {
        float rotation = node->mTextInfo.rangeRotation * factor;
        rawCanvas->Rotate(rotation, 0, 0);
    }

    // 应用范围内的缩放
    if (node->mTextInfo.hasRangeScale) {
        float baseScale = 1.0f;
        float scaleX = baseScale + (node->mTextInfo.rangeScaleX / 100.0f - baseScale) * factor;
        float scaleY = baseScale + (node->mTextInfo.rangeScaleY / 100.0f - baseScale) * factor;
        rawCanvas->Scale(scaleX, scaleY);
    }

    // 应用范围内的位置偏移
    if (node->mTextInfo.hasRangePosition) {
        rawCanvas->Translate(node->mTextInfo.rangePositionX * factor, node->mTextInfo.rangePositionY * factor);
    }
}

HSBColor LottieRenderNode::RgbToHsb(uint8_t r, uint8_t g, uint8_t b)
{
    float rf = r / 255.0f;
    float gf = g / 255.0f;
    float bf = b / 255.0f;

    float maxVal = std::max({rf, gf, bf});
    float minVal = std::min({rf, gf, bf});
    float delta = maxVal - minVal;

    HSBColor hsb;

    // 计算亮度 (Brightness)
    hsb.b = maxVal * 100.0f;

    // 计算饱和度 (Saturation)
    if (maxVal == 0.0f) {
        hsb.s = 0.0f;
    } else if (maxVal > 0) {
        hsb.s = (delta / maxVal) * 100.0f;
    }

    // 计算色相 (Hue)
    if (delta == 0.0f) {
        hsb.h = 0.0f;
    } else {
        if (maxVal == rf) {
            hsb.h = 60.0f * ((gf - bf) / delta);
        } else if (maxVal == gf) {
            hsb.h = 60.0f * (2.0f + (bf - rf) / delta);
        } else {
            hsb.h = 60.0f * (4.0f + (rf - gf) / delta);
        }

        if (hsb.h < 0.0f) {
            hsb.h += 360.0f;
        }
    }

    return hsb;
}

uint32_t LottieRenderNode::HsbToRgb(const HSBColor &hsb, uint8_t alpha)
{
    float h = hsb.h;
    float s = hsb.s / 100.0f;
    float b = hsb.b / 100.0f;

    while (h >= 360.0f)
        h -= 360.0f;
    while (h < 0.0f)
        h += 360.0f;

    float c = b * s;
    float x = c * (1.0f - std::abs(std::fmod(h / 60.0f, 2.0f) - 1.0f));
    float m = b - c;

    float rf = 0.0f;
    float gf = 0.0f;
    float bf = 0.0f;

    if (h < 60.0f) {
        rf = c;
        gf = x;
        bf = 0.0f;
    } else if (h < 120.0f) {
        rf = x;
        gf = c;
        bf = 0.0f;
    } else if (h < 180.0f) {
        rf = 0.0f;
        gf = c;
        bf = x;
    } else if (h < 240.0f) {
        rf = 0.0f;
        gf = x;
        bf = c;
    } else if (h < 300.0f) {
        rf = x;
        gf = 0.0f;
        bf = c;
    } else {
        rf = c;
        gf = 0.0f;
        bf = x;
    }

    uint8_t R = static_cast<uint8_t>(std::round((rf + m) * 255.0f));
    uint8_t G = static_cast<uint8_t>(std::round((gf + m) * 255.0f));
    uint8_t B = static_cast<uint8_t>(std::round((bf + m) * 255.0f));

    return drawing::Color::SetArgb(alpha, R, G, B);
}

// 计算HSB调整后的颜色
uint32_t LottieRenderNode::CalculateHSBAdjustedColor(const ColorRangeParams &rangeParams,
                                                     const HSBAdjustParams &hsbParams, const BaseColorInfo &baseColor,
                                                     float factor)
{
    // 转换为 HSB 色彩空间
    HSBColor hsb = RgbToHsb(static_cast<uint8_t>(rangeParams.rangeFillR), static_cast<uint8_t>(rangeParams.rangeFillG),
                            static_cast<uint8_t>(rangeParams.rangeFillB));

    // 应用色相调整
    if (hsbParams.hasRangeFillHue) {
        hsb.h += hsbParams.rangeFillHue * factor;
        // 确保色相在 0-360 范围内
        while (hsb.h >= 360.0f)
            hsb.h -= 360.0f;
        while (hsb.h < 0.0f)
            hsb.h += 360.0f;
    }

    // 应用饱和度调整
    if (hsbParams.hasRangeFillSaturation) {
        hsb.s += hsbParams.rangeFillSaturation * factor;
        hsb.s = std::max(0.0f, std::min(100.0f, hsb.s));
    }

    // 应用亮度调整
    if (hsbParams.hasRangeFillBrightness) {
        hsb.b += hsbParams.rangeFillBrightness * factor;
        hsb.b = std::max(0.0f, std::min(100.0f, hsb.b));
    }

    // 计算透明度
    float a = static_cast<float>(baseColor.a);
    if (rangeParams.hasRangeOpacity) {
        float opacityFactor = rangeParams.rangeOpacity / 100.0f;
        a = a * (opacityFactor * factor + (1.0f - factor));
    }
    a = std::max(0.0f, std::min(255.0f, a));

    // 转换回 RGB 并设置颜色
    return HsbToRgb(hsb, static_cast<uint8_t>(a));
}

// 计算颜色插值
uint32_t LottieRenderNode::CalculateColorInterpolation(const ColorRangeParams &rangeParams,
                                                       const BaseColorInfo &baseColor, float factor)
{
    // 颜色插值逻辑
    float r = static_cast<float>(baseColor.r) + (rangeParams.rangeFillR - static_cast<float>(baseColor.r)) * factor;
    float g = static_cast<float>(baseColor.g) + (rangeParams.rangeFillG - static_cast<float>(baseColor.g)) * factor;
    float b = static_cast<float>(baseColor.b) + (rangeParams.rangeFillB - static_cast<float>(baseColor.b)) * factor;
    float a = static_cast<float>(baseColor.a);

    // 应用透明度变化
    if (rangeParams.hasRangeOpacity) {
        float opacityFactor = rangeParams.rangeOpacity / 100.0f;
        a = a * (opacityFactor * factor + (1.0f - factor));
    }

    // 确保颜色值在有效范围内
    r = std::max(0.0f, std::min(255.0f, r));
    g = std::max(0.0f, std::min(255.0f, g));
    b = std::max(0.0f, std::min(255.0f, b));
    a = std::max(0.0f, std::min(255.0f, a));

    return drawing::Color::SetArgb(static_cast<uint32_t>(a), static_cast<uint32_t>(r), static_cast<uint32_t>(g),
                                   static_cast<uint32_t>(b));
}

void LottieRenderNode::SetTextRangeColor(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node, float factor)
{
    brush_->Reset();

    // 准备基础颜色信息
    BaseColorInfo baseColor = {node->mTextInfo.r, node->mTextInfo.g, node->mTextInfo.b, node->mTextInfo.a};

    uint32_t textColor;

    if (node->mTextInfo.hasRangeFillColor) {
        // 准备颜色范围参数
        ColorRangeParams rangeParams = {node->mTextInfo.hasRangeFillColor,
                                        static_cast<float>(node->mTextInfo.rangeFillR),
                                        static_cast<float>(node->mTextInfo.rangeFillG),
                                        static_cast<float>(node->mTextInfo.rangeFillB),
                                        node->mTextInfo.hasRangeOpacity,
                                        node->mTextInfo.rangeOpacity};

        // 检查是否需要HSB调整
        if (node->mTextInfo.hasRangeFillHue || node->mTextInfo.hasRangeFillSaturation ||
            node->mTextInfo.hasRangeFillBrightness) {
            HSBAdjustParams hsbParams = {node->mTextInfo.hasRangeFillHue,        node->mTextInfo.hasRangeFillSaturation,
                                         node->mTextInfo.hasRangeFillBrightness, node->mTextInfo.rangeFillHue,
                                         node->mTextInfo.rangeFillSaturation,    node->mTextInfo.rangeFillBrightness};

            textColor = CalculateHSBAdjustedColor(rangeParams, hsbParams, baseColor, factor);
        } else {
            textColor = CalculateColorInterpolation(rangeParams, baseColor, factor);
        }
    } else {
        // 只应用透明度变化
        float a = static_cast<float>(baseColor.a);
        if (node->mTextInfo.hasRangeOpacity) {
            float opacityFactor = node->mTextInfo.rangeOpacity / 100.0f;
            a = a * (opacityFactor * factor + (1.0f - factor));
        }

        a = std::max(0.0f, std::min(255.0f, a));
        textColor = drawing::Color::SetArgb(static_cast<uint32_t>(a), baseColor.r, baseColor.g, baseColor.b);
    }

    brush_->SetColor(textColor);
    rawCanvas->AttachBrush(*brush_);
}

void LottieRenderNode::ApplyTextRangeStroke(std::shared_ptr<drawing::Canvas> rawCanvas,
                                            const drawing::TextBlob *textBlob, const TextPosition &position,
                                            const LOTNode *node, float factor)
{
    if (!node->mTextInfo.hasStroke) {
        return;
    }
    bool hasRangeStroke = node->mTextInfo.hasRangeStrokeColor || node->mTextInfo.hasRangeStrokeWidth;
    if (!hasRangeStroke) {
        // 使用原有描边逻辑
        ApplyTextStroke(rawCanvas, textBlob, position, node);
        return;
    }

    rawCanvas->Save();
    pen_->Reset();

    // 设置描边颜色
    if (node->mTextInfo.hasRangeStrokeColor) {
        float sr = node->mTextInfo.sr + (node->mTextInfo.rangeStrokeR - node->mTextInfo.sr) * factor;
        float sg = node->mTextInfo.sg + (node->mTextInfo.rangeStrokeG - node->mTextInfo.sg) * factor;
        float sb = node->mTextInfo.sb + (node->mTextInfo.rangeStrokeB - node->mTextInfo.sb) * factor;
        uint32_t strokeColor = drawing::Color::SetArgb(node->mTextInfo.sa, sr, sg, sb);
        pen_->SetColor(strokeColor);
    } else {
        uint32_t strokeColor =
            drawing::Color::SetArgb(node->mTextInfo.sa, node->mTextInfo.sr, node->mTextInfo.sg, node->mTextInfo.sb);
        pen_->SetColor(strokeColor);
    }

    // 设置描边宽度
    float strokeWidth = node->mTextInfo.strokeWidth;
    if (node->mTextInfo.hasRangeStrokeWidth) {
        strokeWidth = strokeWidth + (node->mTextInfo.rangeStrokeWidth - strokeWidth) * factor;
    }
    pen_->SetWidth(strokeWidth);
    pen_->SetAntiAlias(true);

    rawCanvas->AttachPen(*pen_);
    rawCanvas->DrawTextBlob(textBlob->get(), position.x, position.y);
    rawCanvas->DetachPen();
    rawCanvas->Restore();
}

std::pair<size_t, size_t> LottieRenderNode::CalculateEffectiveIndex(const LOTNode *node, size_t charIndex,
                                                                    const std::vector<std::string> &processedLines)
{
    switch (node->mTextInfo.basedOn) {
        case NUM_2: // Character Excluding Spaces
            return CalculateNonSpaceIndex(charIndex, processedLines);
        case NUM_3: // Words
            return CalculateWordIndex(charIndex, processedLines);
        case NUM_4: // Lines
            return CalculateLineIndex(charIndex, processedLines);
        case NUM_1: // Characters
        default:
            // 默认按字符处理
            size_t totalChars = 0;
            for (const auto &line : processedLines) {
                for (size_t i = 0; i < line.length();) {
                    i += GetUTF8CharBytes(line[i]);
                    totalChars++;
                }
            }
            return {charIndex, totalChars};
    }
}

std::pair<size_t, size_t> LottieRenderNode::CalculateNonSpaceIndex(size_t charIndex,
                                                                   const std::vector<std::string> &processedLines)
{
    size_t nonSpaceIndex = 0;
    size_t totalNonSpaceChars = 0;
    size_t currentIndex = 0;
    bool foundTarget = false;

    for (const auto &line : processedLines) {
        for (size_t i = 0; i < line.length();) {
            int charBytes = GetUTF8CharBytes(line[i]);
            std::string currentChar = line.substr(i, charBytes);

            // 检查是否为空格字符（包括空格、制表符、换行符等）
            bool isSpace = (currentChar == " " || currentChar == "\t" || currentChar == "\n" || currentChar == "\r");

            if (!isSpace && currentIndex == charIndex) {
                nonSpaceIndex = totalNonSpaceChars;
                foundTarget = true;
                totalNonSpaceChars++;
            } else if (!isSpace) {
                totalNonSpaceChars++;
            }
            i += charBytes;
            currentIndex++;
        }
    }

    // 如果没找到目标索引（可能是空格字符），使用最接近的非空格字符索引
    if (!foundTarget) {
        nonSpaceIndex = (totalNonSpaceChars > 0) ? totalNonSpaceChars - 1 : 0;
    }

    return {nonSpaceIndex, totalNonSpaceChars};
}

std::pair<size_t, size_t> LottieRenderNode::CalculateWordIndex(size_t charIndex,
                                                               const std::vector<std::string> &processedLines)
{
    size_t wordIndex = 0;
    size_t totalWords = 0;
    size_t currentIndex = 0;
    bool inWord = false;

    for (const auto &line : processedLines) {
        for (size_t i = 0; i < line.length();) {
            int charBytes = GetUTF8CharBytes(line[i]);
            std::string currentChar = line.substr(i, charBytes);

            // 检查是否为分隔符（空格、标点等）
            bool isSeparator = (currentChar == " " || currentChar == "\t" || currentChar == "\n" ||
                                currentChar == "\r" || currentChar == "," || currentChar == "." || currentChar == "!" ||
                                currentChar == "?" || currentChar == ";" || currentChar == ":");

            if (!isSeparator && !inWord && currentIndex <= charIndex) {
                inWord = true;
                wordIndex = totalWords;
                totalWords++;
            } else if (!isSeparator && !inWord && currentIndex > charIndex) {
                inWord = true;
                totalWords++;
            } else if (isSeparator && inWord) {
                // 单词结束
                inWord = false;
            }

            i += charBytes;
            currentIndex++;
        }
    }

    return {wordIndex, totalWords};
}

std::pair<size_t, size_t> LottieRenderNode::CalculateLineIndex(size_t charIndex,
                                                               const std::vector<std::string> &processedLines)
{
    size_t lineIndex = 0;
    size_t totalLines = processedLines.size();
    size_t currentIndex = 0;

    for (size_t lineNum = 0; lineNum < processedLines.size(); lineNum++) {
        const auto &line = processedLines[lineNum];
        size_t lineCharCount = 0;

        // 计算当前行的字符数
        for (size_t i = 0; i < line.length();) {
            i += GetUTF8CharBytes(line[i]);
            lineCharCount++;
        }

        // 检查目标字符是否在当前行
        if (charIndex >= currentIndex && charIndex < currentIndex + lineCharCount) {
            lineIndex = lineNum;
            break;
        }

        currentIndex += lineCharCount;
    }

    return {lineIndex, totalLines};
}
void LottieRenderNode::ProcessImageNode(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node)
{
    drawing::Bitmap bitmap;
    OH_Drawing_Image_Info imageInfo = {static_cast<int32_t>(node->mImageInfo.width),
                                       static_cast<int32_t>(node->mImageInfo.height), COLOR_FORMAT_BGRA_8888,
                                       ALPHA_FORMAT_PREMUL};
    bitmap = bitmap.CreateFromPixels(&imageInfo, node->mImageInfo.data, node->mImageInfo.width * NUM_4);
    rawCanvas->Save();
    drawing::Matrix matrix;
    matrix.SetMatrix({node->mImageInfo.mMatrix.m11, node->mImageInfo.mMatrix.m21, node->mImageInfo.mMatrix.m31,
                      node->mImageInfo.mMatrix.m12, node->mImageInfo.mMatrix.m22, node->mImageInfo.mMatrix.m32,
                      node->mImageInfo.mMatrix.m13, node->mImageInfo.mMatrix.m23, node->mImageInfo.mMatrix.m33});
    rawCanvas->ConcatMatrix(matrix.get());
    OH_Drawing_Rect *imageRect = OH_Drawing_RectCreate(0, 0, node->mImageInfo.width, node->mImageInfo.height);
    OH_Drawing_SamplingOptions *samplingOptions = OH_Drawing_SamplingOptionsCreate(
        OH_Drawing_FilterMode::FILTER_MODE_LINEAR, OH_Drawing_MipmapMode::MIPMAP_MODE_NEAREST);
    rawCanvas->DrawBitmapRect(bitmap.get(), imageRect, imageRect, samplingOptions);
    OH_Drawing_RectDestroy(imageRect);
    rawCanvas->Restore();
}

void LottieRenderNode::SetupBrush(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node)
{
    if (!node || !rawCanvas) {
        return;
    }
    brush_->Reset();
    switch (node->mBrushType) {
        case BrushSolid: {
            // 纯色填充
            uint32_t color = drawing::Color::SetArgb(node->mColor.a, node->mColor.r, node->mColor.g, node->mColor.b);
            brush_->SetColor(color);
            break;
        }
        case BrushGradient: {
            // 渐变填充
            if (!node->mGradient.stopPtr || node->mGradient.stopCount <= 0) {
                break;
            }
            // 准备渐变色和位置数组
            std::vector<uint32_t> colors(node->mGradient.stopCount);
            std::vector<float> positions(node->mGradient.stopCount);
            for (size_t i = 0; i < node->mGradient.stopCount; i++) {
                colors[i] = drawing::Color::SetArgb(node->mGradient.stopPtr[i].a, node->mGradient.stopPtr[i].r,
                                                    node->mGradient.stopPtr[i].g, node->mGradient.stopPtr[i].b);
                positions[i] = node->mGradient.stopPtr[i].pos;
            }
            if (node->mGradient.type == GradientLinear) {
                OH_Drawing_Point2D startPt = {node->mGradient.start.x, node->mGradient.start.y};
                OH_Drawing_Point2D endPt = {node->mGradient.end.x, node->mGradient.end.y};
                brush_->SetLinearShaderEffect({&startPt, &endPt, colors.data(), positions.data(),
                                               (uint32_t)node->mGradient.stopCount, CLAMP, nullptr});
            } else if (node->mGradient.type == GradientRadial) {
                OH_Drawing_Point2D centerPt = {node->mGradient.center.x, node->mGradient.center.y};
                OH_Drawing_Point2D endPt = {node->mGradient.focal.x, node->mGradient.focal.y};
                brush_->SetRadialShaderEffect({&centerPt, node->mGradient.cradius, &endPt, node->mGradient.fradius,
                                               colors.data(), positions.data(), (uint32_t)node->mGradient.stopCount,
                                               CLAMP, nullptr});
            }
            break;
        }
        default:
            DRAWING_LOGE("LottieRenderNode::SetupBrush: Unknown brush type");
            break;
    }
    brush_->SetAntiAlias(true);
    AttachBrush(rawCanvas);
}

void LottieRenderNode::SetupPenColor(const LOTNode *node)
{
    // 设置线条颜色
    switch (node->mBrushType) {
        case BrushSolid: {
            // 纯色填充
            uint32_t color = drawing::Color::SetArgb(node->mColor.a, node->mColor.r, node->mColor.g, node->mColor.b);
            pen_->SetColor(color);
            break;
        }
        case BrushGradient: {
            // 渐变填充
            if (!node->mGradient.stopPtr || node->mGradient.stopCount <= 0) {
                break;
            }
            // 准备渐变色和位置数组
            std::vector<uint32_t> colors(node->mGradient.stopCount);
            std::vector<float> positions(node->mGradient.stopCount);
            for (size_t i = 0; i < node->mGradient.stopCount; i++) {
                colors[i] = drawing::Color::SetArgb(node->mGradient.stopPtr[i].a, node->mGradient.stopPtr[i].r,
                                                    node->mGradient.stopPtr[i].g, node->mGradient.stopPtr[i].b);
                positions[i] = node->mGradient.stopPtr[i].pos;
            }
            if (node->mGradient.type == GradientLinear) {
                OH_Drawing_Point2D startPt = {node->mGradient.start.x, node->mGradient.start.y};
                OH_Drawing_Point2D endPt = {node->mGradient.end.x, node->mGradient.end.y};
                pen_->SetLinearShaderEffect({&startPt, &endPt, colors.data(), positions.data(),
                                             (uint32_t)node->mGradient.stopCount, CLAMP, nullptr});
            } else if (node->mGradient.type == GradientRadial) {
                OH_Drawing_Point2D centerPt = {node->mGradient.center.x, node->mGradient.center.y};
                OH_Drawing_Point2D endPt = {node->mGradient.focal.x, node->mGradient.focal.y};
                pen_->SetRadialShaderEffect({&centerPt, node->mGradient.cradius, &endPt, node->mGradient.fradius,
                                             colors.data(), positions.data(), (uint32_t)node->mGradient.stopCount,
                                             CLAMP, nullptr});
            }
            break;
        }
        default:
            DRAWING_LOGE("LottieRenderNode::SetupPenColor: Unknown brush type");
            break;
    }
}

void LottieRenderNode::ConfigurePenStyle(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node)
{
    // 设置线条宽度
    pen_->SetWidth(node->mStroke.width);

    // 设置线条端点样式
    drawing::Pen::LineCapStyle capStyle;
    switch (node->mStroke.cap) {
        case CapSquare:
            capStyle = LINE_SQUARE_CAP;
            break;
        case CapRound:
            capStyle = LINE_ROUND_CAP;
            break;
        case CapFlat:
        default:
            capStyle = LINE_FLAT_CAP;
            break;
    }
    pen_->SetLineCap(capStyle);

    // 设置线条连接样式
    drawing::Pen::LineJoinStyle joinStyle;
    switch (node->mStroke.join) {
        case JoinBevel:
            joinStyle = LINE_BEVEL_JOIN;
            break;
        case JoinRound:
            joinStyle = LINE_ROUND_JOIN;
            break;
        case JoinMiter:
        default:
            joinStyle = LINE_MITER_JOIN;
            break;
    }
    pen_->SetLineJoin(joinStyle);

    // 设置斜接限制
    pen_->SetMiterLimit(node->mStroke.miterLimit);
    pen_->SetAntiAlias(true);
    // 附加画笔到画布
    rawCanvas->AttachPen(*pen_);
}

void LottieRenderNode::SetupPen(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node)
{
    if (!node || !rawCanvas || !node->mStroke.enable) {
        return;
    }

    pen_->Reset();

    SetupPenColor(node);
    ConfigurePenStyle(rawCanvas, node);
}

void LottieRenderNode::ConvertLottiePathToDrawingPath(const float *ptPtr, size_t ptCount, const char *elmPtr,
                                                      size_t elmCount, drawing::Path &path)
{
    if (!ptPtr || !elmPtr || ptCount < NUM_2 || elmCount < NUM_1) {
        return;
    }

    size_t ptIndex = 0;
    for (size_t i = 0; i < elmCount; i++) {
        switch (elmPtr[i]) {
            case NUM_0: // MoveTo
                if (ptIndex + 1 < ptCount) {
                    path.MoveTo(ptPtr[ptIndex], ptPtr[ptIndex + 1]);
                    ptIndex += NUM_2;
                }
                break;
            case NUM_1: // LineTo
                if (ptIndex + 1 < ptCount) {
                    path.LineTo(ptPtr[ptIndex], ptPtr[ptIndex + 1]);
                    ptIndex += NUM_2;
                }
                break;
            case NUM_2: // CubicTo
                if (ptIndex + NUM_5 < ptCount) {
                    path.CubicTo({ptPtr[ptIndex], ptPtr[ptIndex + 1], ptPtr[ptIndex + NUM_2], ptPtr[ptIndex + NUM_3],
                                  ptPtr[ptIndex + NUM_4], ptPtr[ptIndex + NUM_5]});
                    ptIndex += NUM_6;
                }
                break;
            case NUM_4: // QuadTo
                if (ptIndex + NUM_3 < ptCount) {
                    path.QuadTo({ptPtr[ptIndex], ptPtr[ptIndex + 1], ptPtr[ptIndex + NUM_2], ptPtr[ptIndex + NUM_3]});
                    ptIndex += NUM_4;
                }
                break;
            case NUM_3: // Close
                path.Close();
                break;
            default:
                DRAWING_LOGE("LottieRenderNode::ConvertLottiePathToDrawingPath: Unknown path element: %{public}c",
                             elmPtr[i]);
                break;
        }
    }
}

std::shared_ptr<drawing::DrawFilter> LottieRenderNode::SetBluer(std::shared_ptr<drawing::Canvas> rawCanvas, float rx,
                                                                float ry)
{
    auto draw_filter = std::make_shared<drawing::DrawFilter>();
    draw_filter->SetFilter(rx, ry);
    pen_->SetFilter(draw_filter->getFilter());
    rawCanvas->AttachPen(*pen_);
    this->isDetachPen_ = false;
    return draw_filter;
}

void LottieRenderNode::AttachBrush(std::shared_ptr<drawing::Canvas> rawCanvas)
{
    constexpr size_t colorLimits = 255;
    if (colorPercent_) {
        uint32_t color =
            drawing::Color::SetArgb(colorPercent_.get()->opacity * colorLimits, colorPercent_.get()->r * colorLimits,
                                    colorPercent_.get()->g * colorLimits, colorPercent_.get()->b * colorLimits);
        brush_->SetColor(color);
    }
    if (colorBlack_.size() > 0 && colorWhite_.size() > 0) {
        TintEffect(rawCanvas, colorBlack_, colorWhite_, intensity_);
    }
    if (triToneColor_.size() >= NUM_3) {
        TriToneEffect(rawCanvas);
    }
    rawCanvas->AttachBrush(*brush_);
}

void LottieRenderNode::TintEffect(std::shared_ptr<drawing::Canvas> rawCanvas, std::vector<float> colorBlack,
                                  std::vector<float> colorWhite, float intensity)
{
    if (colorBlack.size() > 0 && colorWhite.size() > 0) {
        const float matrix[20] = {(colorWhite[0] - colorBlack[0]),
                                  0,
                                  0,
                                  0,
                                  colorBlack[0],
                                  (colorWhite[1] - colorBlack[1]),
                                  0,
                                  0,
                                  0,
                                  colorBlack[1],
                                  (colorWhite[NUM_2] - colorBlack[NUM_2]),
                                  0,
                                  0,
                                  0,
                                  colorBlack[NUM_2],
                                  0,
                                  0,
                                  0,
                                  1,
                                  0};
        constexpr size_t MaxLength = 20;
        float blendedMatrix[MaxLength];
        // 单位矩阵
        const float identity[MaxLength] = {0.3333, 0, 0, 0, 0, 0, 0.3333, 0, 0, 0, 0, 0, 0.3333, 0, 0, 0, 0, 0, 1, 0};
        for (int i = 0; i < MaxLength; i++) {
            if (i % NUM_5 == NUM_4) { // 偏移量直接线性插值
                blendedMatrix[i] = intensity * matrix[i];
            } else { // 乘数部分混合
                blendedMatrix[i] = (1 - intensity) * identity[i] + intensity * matrix[i];
            }
        }
        auto draw_filter = std::make_shared<drawing::DrawFilter>();
        draw_filter->SetFilterSrgbMatrix(blendedMatrix);
        brush_->SetEffectFilter(draw_filter->getFilter());
        rawCanvas->AttachBrush(*brush_);
    }
}

void LottieRenderNode::TriToneEffect(std::shared_ptr<drawing::Canvas> rawCanvas)
{
    constexpr size_t colorsOffset = 8;
    constexpr size_t colorsOffset1 = 16;
    constexpr size_t colorsOffset2 = 24;
    constexpr size_t colorMask = 0xff;
    constexpr size_t shadow = 85;
    constexpr size_t midtones = 170;
    constexpr float mask = 255.0;

    auto colors = brush_->GetColor();
    auto r = (colors >> colorsOffset1) & colorMask;
    auto g = (colors >> colorsOffset) & colorMask;
    auto b = (colors)&colorMask;
    // 计算灰度值（加权平均）
    uint8_t luminance = static_cast<uint8_t>(0.299 * r + 0.587 * g + 0.114 * b);
    auto shadowColor = triToneColor_[NUM_0];
    auto midToneColor = triToneColor_[NUM_1];
    auto highlightColor = triToneColor_[NUM_2];
    if (luminance < shadow) { // 阴影
        r = shadowColor[NUM_0] * mask;
        g = shadowColor[NUM_1] * mask;
        b = shadowColor[NUM_2] * mask;
    } else if (luminance < midtones) { // 中间调
        r = midToneColor[NUM_0] * mask;
        g = midToneColor[NUM_1] * mask;
        b = midToneColor[NUM_2] * mask;
    } else { // 高光
        r = highlightColor[NUM_0] * mask;
        g = highlightColor[NUM_1] * mask;
        b = highlightColor[NUM_2] * mask;
    }
    uint32_t color = drawing::Color::SetArgb((colors >> colorsOffset2) & colorMask, static_cast<uint32_t>(r),
                                             static_cast<uint32_t>(g), static_cast<uint32_t>(b));
    brush_->SetColor(color);
    rawCanvas->AttachBrush(*brush_);
}

void LottieRenderNode::StrokeEffect(std::shared_ptr<drawing::Canvas> rawCanvas,
                                    std::shared_ptr<drawing::Path> rawClipMask)
{
    if (!rawClipMask || !strokeProps_) {
        rawCanvas->DrawPath(path_->get());
        return;
    }
    constexpr float mask = 255.0;
    rawCanvas->DetachBrush();
    pen_->SetAntiAlias(true);
    if (strokeProps_->showType == DISPLAY_OPACITY || strokeProps_->showType == DISPLAY_BACKGROUND) {
        uint32_t color =
            drawing::Color::SetArgb(strokeProps_.get()->colors[NUM_3] * mask, strokeProps_.get()->colors[NUM_0] * mask,
                                    strokeProps_.get()->colors[NUM_1] * mask, strokeProps_.get()->colors[NUM_2] * mask);
        pen_->SetColor(color);
        if (strokeProps_->showType == DISPLAY_BACKGROUND) {
            rawCanvas->DrawColor(brush_->GetColor(), OH_Drawing_BlendMode::BLEND_MODE_HARD_LIGHT);
        }
    } else {
        pen_->SetColor(brush_->GetColor());
    }
    pen_->SetWidth(strokeProps_.get()->width);
    pen_->SetAlpha(strokeProps_.get()->opacity * mask);
    if (strokeProps_.get()->lineSpace > 0) {
        float space = strokeProps_->width * NUM_2 * strokeProps_.get()->lineSpace * 0.01;
        float intervals[] = {1, space};
        pen_->SetDashPathEffect(intervals, sizeof(intervals) / sizeof(float), 0.0);
    }
    rawCanvas->AttachPen(*pen_);
    rawCanvas->DrawPath(rawClipMask->get());
}

bool LottieRenderNode::isRenderStroke()
{
    return strokeProps_ && strokeProps_->isAllMask &&
           (strokeProps_->showType == DISPLAY_BACKGROUND || strokeProps_->showType == DISPLAY_OPACITY ||
            strokeProps_->showType == DISPLAY_LINE);
}

void LottieRenderNode::ClearEffectOfEf()
{
    // 清除ef的fill效果
    if (colorPercent_) {
        colorPercent_ = nullptr;
    }
    // 清除ef的Tint效果
    if (colorBlack_.size() > 0) {
        std::vector<float>().swap(colorBlack_);
    }
    if (colorWhite_.size() > 0) {
        std::vector<float>().swap(colorWhite_);
    }
    // 清除ef的triTone效果
    if (triToneColor_.size() > 0) {
        std::vector<std::vector<float>>().swap(triToneColor_);
    }
    // 清除ef的stroke效果
    if (strokeProps_) {
        strokeProps_ = nullptr;
    }
}

void LottieRenderNode::ClearRenderInfo()
{
    renderInfo_ = RenderInfo{};
}
} // namespace LottieTurbo
