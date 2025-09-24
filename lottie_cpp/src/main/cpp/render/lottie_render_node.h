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

#ifndef LOTTIETURBO_LOTTIE_RENDER_NODE_H
#define LOTTIETURBO_LOTTIE_RENDER_NODE_H

#include "brush.h"
#include "canvas.h"
#include "draw_filter.h"
#include "font.h"
#include "font_collection.h"
#include "path.h"
#include "rlottiecommon.h"
#include "text_blob.h"
#include "text_typography.h"
#include <native_drawing/drawing_types.h>
#include <cmath>
#include <algorithm>
#include <random>
#include <cmath>
#include <chrono>
#include <set>
#include <utility>
#include <algorithm>

namespace LottieTurbo {

struct ColorPercent {
    float r;
    float g;
    float b;
    float opacity;
};

struct BaseColorInfo {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t a;
};

/**
 * @isAllMask : is open all mask
 * @width     : stroke width
 * @opacity   : line opacity
 * @lineSpace : line space
 * @showType  : show type 1: background 2:opacity 3:line color 4:displays the original
 * @colors    : line color
 */
struct StrokeProps {
    int isAllMask;
    float width;
    float opacity;
    float lineSpace;
    int showType;
    std::vector<float> colors;
};

enum StrokeShowType {
    /**
     * 画布展示layer背景颜色，同时stroke展示设置的颜色
     */
    DISPLAY_BACKGROUND = 1,
    /**
     * 画布展示透明颜色，同时stroke展示设置的颜色
     */
    DISPLAY_OPACITY = 2,
    /**
     * 画布展示透明颜色，同时stroke展示设置layer的颜色
     */
    DISPLAY_LINE = 3,
};

struct TextPosition {
    float x;
    float y;
};

struct TextDrawParams {
    float tracking;
    TextPosition position;
};

struct CharDrawParams {
    TextPosition position;
    float tracking;
    const char* singleChar;
    int charBytes;
};

struct HSBColor {
    float h; // 色相
    float s; // 饱和度
    float b; // 亮度
};

struct TextRenderParams {
    float lineHeight;
    int textJustify;
    float baselineShift;
    float scaledTracking;
    bool hasTextBox;
    float textBoxWidth;
};

struct TextRangeInfo {
    bool isInRange;
    float factor;                // 0.0 - 1.0，表示在范围内的程度
};

struct CharPositionInfo {
    std::string singleChar;
    TextRangeInfo rangeInfo;
    float& currentX;
    float y;
    float scaledTracking;
};

struct TextProcessConfig {
    float textBoxWidth;
    float textBoxHeight;
    float textBoxPosX;
    float textBoxPosY;
    bool hasTextBox;
    float lineHeight;
    int textJustify;
    float baselineShift;
    float tracking;
    float scaledTracking;
};

struct TextRangeParams {
    float start;
    float end;
    float offset;
    size_t effectiveIndex;
    size_t effectiveTotal;
    bool hasRandomize;
    int randomize;
};

struct ColorRangeParams {
    bool hasRangeFillColor;
    float rangeFillR;
    float rangeFillG;
    float rangeFillB;
    bool hasRangeOpacity;
    float rangeOpacity;
};

struct HSBAdjustParams {
    bool hasRangeFillHue;
    bool hasRangeFillSaturation;
    bool hasRangeFillBrightness;
    float rangeFillHue;
    float rangeFillSaturation;
    float rangeFillBrightness;
};

struct TextPathOffset {
    float horizontal;  // 水平偏移
    float vertical;    // 垂直偏移

    TextPathOffset(float h = 0.0f, float v = 0.0f) : horizontal(h), vertical(v) {}
};

struct RenderInfo {
    size_t savelayer{0};
    size_t text{0};
    size_t path{0};
    size_t image{0};
    size_t mask{0};
    size_t blurEff{0};
    size_t fillEff{0};
    size_t tintEff{0};
    size_t tritoneEff{0};
    size_t strokeEff{0};
};
class LottieRenderNode {
public:
    LottieRenderNode();
    ~LottieRenderNode();
    RenderInfo renderInfo_;

    /**
     * @brief 清除已记录的绘制信息
     */
    void ClearRenderInfo();
    
    /**
     * @brief 设置当前图层节点
     * @param layerNode 当前处理的图层节点
     */
    void SetCurrentTextLayerNode(const LOTLayerNode* layerNode);

    /**
     * @brief 获取当前图层节点
     * @return 当前图层节点指针，如果未设置则返回nullptr
     */
    const LOTLayerNode* GetCurrentTextLayerNode() const;

    /**
     * 处理单个节点（路径、填充等）
     * @param canvas
     * @param node
     * @param clipMask
     */
    void ProcessNode(std::shared_ptr<drawing::Canvas> canvas, const LOTNode *node,
                     std::shared_ptr<drawing::Path> clipMask = nullptr);

    /**
     * 设置画刷
     * @param canvas
     * @param node
     */
    void SetupBrush(std::shared_ptr<drawing::Canvas> canvas, const LOTNode *node);

    /**
     * 设置画笔颜色
     * @param node
     */
    void SetupPenColor(const LOTNode *node);

    /**
     * 设置画笔样式
     * @param rawCanvas
     * @param node
     */
    void ConfigurePenStyle(std::shared_ptr<drawing::Canvas> rawCanvas, const LOTNode *node);

    /**
     * 设置画笔
     * @param canvas
     * @param node
     */
    void SetupPen(std::shared_ptr<drawing::Canvas> canvas, const LOTNode *node);

    /**
     * 绘制路径
     * @param ptPtr
     * @param ptCount
     * @param elmPtr
     * @param elmCount
     * @return
     */
    void ConvertLottiePathToDrawingPath(const float *ptPtr, size_t ptCount, const char *elmPtr, size_t elmCount,
                                        drawing::Path &path);

    /**
     * 高斯模糊
     * @param rawCanvas
     * @param rx x轴方向模糊的偏移量
     * @param ry y轴方向模糊的偏移量
     * @return
     */
    std::shared_ptr<drawing::DrawFilter> SetBluer(std::shared_ptr<drawing::Canvas> rawCanvas, float rx, float ry);

    /**
     * 设置滤镜颜色
     * @param percent
     */
    void SetEffectColorPercent(std::shared_ptr<ColorPercent> percent)
    {
        colorPercent_ = std::move(percent);
    }

    /**
     *  黑白颜色组
     * @param color_Black
     * @param color_White
     * @param ins
     */

    void SetTintColor(std::vector<float> color_Black, std::vector<float> color_White, float ins)
    {
        colorBlack_ = std::move(color_Black);
        colorWhite_ = std::move(color_White);
        intensity_ = ins / 100.0f;
    }

    /**
     * 三色调
     * @param color
     */
    void SetTriToneColor(std::vector<std::vector<float>> color)
    {
        triToneColor_ = std::move(color);
    }

    /**
     * 设置stroke props
     * @param percent
     */
    void SetEffectStrokeProps(std::shared_ptr<StrokeProps> props)
    {
        strokeProps_ = std::move(props);
    }
    
    /**
     * 清除layer层级ef属性设置
     */
    void ClearEffectOfEf();

private:
    /**
     * @brief 处理文本路径渲染
     * @param rawCanvas 画布
     * @param node 文本节点
     * @param layerNode 图层节点
     * @return 是否成功处理了文本路径
     */
    bool ProcessTextPath(std::shared_ptr<drawing::Canvas> rawCanvas,
                         const LOTNode* node,
                         const LOTLayerNode* layerNode);
    /**
     * @brief 创建文本路径的Typography对象
     * @param node 文本节点
     * @param fontCollection 字体集合
     * @param typographyStyle Typography样式
     * @return Typography对象
     */
    drawing::Typography CreateTextPathTypography(const LOTNode* node,
                                                 drawing::FontCollection& fontCollection,
                                                 drawing::TypographyStyle& typographyStyle);

    /**
     * @brief 验证参数并准备文本路径
     * @param node 文本节点
     * @param layerNode 图层节点
     * @return 路径对象，失败返回nullptr
     */
    std::shared_ptr<drawing::Path> ValidateAndPreparePath(const LOTNode* node,
                                                          const LOTLayerNode* layerNode);

    /**
     * @brief 计算Typography布局宽度
     * @param node 文本节点
     * @return 计算得出的布局宽度
     */
    float CalculateTypographyLayoutWidth(const LOTNode* node);

    /**
     * @brief 创建文本路径Typography对象
     * @param node 文本节点
     * @return Typography对象
     */
    drawing::Typography CreateTextPathTypographyObject(const LOTNode* node);

    /**
     * @brief 在路径上渲染文本
     * @param rawCanvas 画布
     * @param node 文本节点
     * @param pathObj 路径对象
     * @param typography Typography对象
     * @return 是否成功
     */
    bool RenderTextOnPath(std::shared_ptr<drawing::Canvas> rawCanvas,
                          const LOTNode* node,
                          std::shared_ptr<drawing::Path> pathObj,
                          drawing::Typography& typography);

    /**
     * @brief 渲染文本路径描边
     * @param rawCanvas 画布
     * @param node 文本节点
     * @param pathObj 路径对象
     * @param typography Typography对象
     * @param hOffset 水平偏移
     * @param vOffset 垂直偏移
     */
    void RenderTextPathStroke(std::shared_ptr<drawing::Canvas> rawCanvas,
                             const LOTNode* node,
                             std::shared_ptr<drawing::Path> pathObj,
                             drawing::Typography& typography,
                             const TextPathOffset& offset);
    /**
     * 处理路径节点
     * @param canvas
     * @param node
     * @param clipMask
     */
    void ProcessPathNode(std::shared_ptr<drawing::Canvas> canvas, const LOTNode *node,
                         std::shared_ptr<drawing::Path> clipMask);

    /**
     * @brief 初始化文本字体
     * @param node 节点信息
     * @return 初始化的字体对象
     */
    std::unique_ptr<drawing::Font> InitializeFont(const LOTNode* node);

    /**
     * @brief 分割文本为多行
     * @param text 原始文本
     * @return 分割后的文本行
     */
    std::vector<std::string> SplitTextIntoLines(const char* text);

    /**
     * @brief 创建文本blob并绘制（无字距调整）
     * @param rawCanvas 画布
     * @param font 字体
     * @param line 文本行
     * @param params 绘制参数
     * @param node 节点信息
     */
    void DrawTextBlob(std::shared_ptr<drawing::Canvas> rawCanvas, drawing::Font* font,
                      const std::string& line,
                      const TextDrawParams& params,
                      const LOTNode* node);

    /**
     * @brief 提取UTF-8字符
     * @param line 输入的文本字符串
     * @param index 要提取的字符位置
     * @param outChar 输出字符缓冲区
     * @param outCharSize 输出缓冲区大小
     * @return 提取的UTF-8字符字节数
     */
    int ExtractUtf8Char(const std::string& line, size_t index, char* outChar, size_t outCharSize);

    /**
     * @brief 绘制单个字符
     * @param rawCanvas 画布
     * @param font 字体
     * @param params 字符绘制参数
     * @param node 节点信息
     * @param outNextX 输出下一个字符的X坐标位置
     */
    void DrawSingleChar(std::shared_ptr<drawing::Canvas> rawCanvas,
                        drawing::Font* font,
                        const CharDrawParams& params,
                        const LOTNode* node,
                        float& outNextX);

    /**
     * @brief 逐字符绘制文本（有字距调整）
     * @param rawCanvas 画布
     * @param font 字体
     * @param line 文本行
     * @param params 绘制参数
     * @param node 节点信息
     */
    void DrawCharByChar(std::shared_ptr<drawing::Canvas> rawCanvas,
                        drawing::Font* font,
                        const std::string& line,
                        const TextDrawParams& params,
                        const LOTNode* node);

    /**
     * @brief 绘制单行文本
     * @param rawCanvas 画布
     * @param font 字体
     * @param line 文本行内容
     * @param TextDrawParams 绘制参数
     * @param node 节点信息
     */
    void DrawSingleLine(std::shared_ptr<drawing::Canvas> rawCanvas,
                        drawing::Font* font,
                        const std::string& line,
                        const TextDrawParams& params,
                        const LOTNode* node);

    /**
     * @brief 查找文本行的断点位置
     * @param font
     * @param line
     * @param maxWidth
     * @param tracking
     * @return
     */

    size_t FindBreakPoint(drawing::Font* font, const std::string& line, float maxWidth, float tracking);

    /**
     * @brief 获取UTF-8字符的字节数
     * @param firstByte 第一个字节
     * @return 字符的字节数
     */

    int GetUTF8CharBytes(char firstByte);
    /**
     * @brief 处理文本描边
     * @param rawCanvas 画布
     * @param textBlob 文本对象
     * @param position 文本位置
     * @param node 节点信息
     */

    void ApplyTextStroke(std::shared_ptr<drawing::Canvas> rawCanvas,
                         const drawing::TextBlob* textBlob,
                         const TextPosition& position,
                         const LOTNode* node);

    /**
     * @brief 初始化文本处理配置
     * @param rawCanvas 画布
     * @param node 节点信息
     * @param font 字体对象引用（输出参数）
     * @return 文本处理配置
     */
    TextProcessConfig InitializeTextProcessing(std::shared_ptr<drawing::Canvas> rawCanvas,
                                              const LOTNode* node,
                                              std::unique_ptr<drawing::Font>& font);

    /**
     * @brief 处理文本换行
     * @param node 节点信息
     * @param font 字体对象
     * @param config 文本处理配置
     * @return 处理后的文本行
     */
    std::vector<std::string> ProcessTextWrapping(const LOTNode* node,
                                                 drawing::Font* font,
                                                 const TextProcessConfig& config);

    /**
     * @brief 执行文本渲染
     * @param rawCanvas 画布
     * @param font 字体对象
     * @param processedLines 处理后的文本行
     * @param node 节点信息
     * @param config 文本处理配置
     */
    void ExecuteTextRendering(std::shared_ptr<drawing::Canvas> rawCanvas,
                             drawing::Font* font,
                             const std::vector<std::string>& processedLines,
                             const LOTNode* node,
                             const TextProcessConfig& config);

    /**
     * 处理文本节点
     * @param canvas
     * @param node
     */
    void ProcessTextNode(std::shared_ptr<drawing::Canvas> canvas, const LOTNode *node);

    /**
     * 处理图像节点
     * @param canvas
     * @param node
     */
    void ProcessImageNode(std::shared_ptr<drawing::Canvas> canvas, const LOTNode *node);

    /**
     * 统一设置AttachBrush
     */
    void AttachBrush(std::shared_ptr<drawing::Canvas> canvas);

    /**
     * 设置TriToneEffect效果
     */
    void TriToneEffect(std::shared_ptr<drawing::Canvas> rawCanvas);

    /**
     * 设置TintEffect效果
     */
    void TintEffect(std::shared_ptr<drawing::Canvas> rawCanvas, std::vector<float> color_Black,
                    std::vector<float> color_White, float ins);

    /**
     * 设置StrokeEffect效果
     */
    void StrokeEffect(std::shared_ptr<drawing::Canvas> rawCanvas, std::shared_ptr<drawing::Path> rawClipMask);

    /**
     * 是否走StrokeEffect流程
     */
    bool isRenderStroke();

    std::shared_ptr<drawing::Brush> brush_;
    std::shared_ptr<drawing::Pen> pen_;
    std::shared_ptr<drawing::Path> path_;
    bool isDetachPen_{true};
    std::shared_ptr<ColorPercent> colorPercent_{nullptr};
    std::vector<float> colorBlack_;
    std::vector<float> colorWhite_;
    std::vector<std::vector<float>> triToneColor_;
    float intensity_;
    std::shared_ptr<StrokeProps> strokeProps_{nullptr};
    long randomNum_ = 0;
    const LOTLayerNode* currentLayerNode_;
    /**
     * @brief 计算文本范围参数
     * @param node 文本节点
     * @param charIndex 字符索引
     * @param processedLines 处理后的文本行
     * @return 文本范围参数
     */
    TextRangeParams CalculateTextRangeParams(const LOTNode* node,
                                             size_t charIndex,
                                             const std::vector<std::string>& processedLines);

    /**
     * @brief 应用形状因子和其他效果
     * @param node 文本节点
     * @param relativePosition 相对位置 (0.0 - 1.0)
     * @return 最终的因子值
     */
    float ApplyShapeAndEffects(const LOTNode* node, float relativePosition);

    /**
     * @brief 计算文本范围信息（包含随机化和 basedOn 字段）
     * @param node 文本节点
     * @param charIndex 字符索引
     * @param totalChars 总字符数
     * @param processedLines 处理后的文本行
     * @return 文本范围信息
     */
    TextRangeInfo CalculateTextRangeInfo(const LOTNode* node,
                                         size_t charIndex,
                                         const std::vector<std::string>& processedLines);

    /**
     * @brief 计算文本行的布局和对齐
     * @param font 字体
     * @param line 文本行
     * @param params 渲染参数
     * @param startX 输出的起始X坐标
     * @param textWidth 输出的文本宽度
     */
    void CalculateTextLineLayout(drawing::Font* font,
                                 const std::string& line,
                                 const TextRenderParams& params,
                                 float& startX,
                                 float& textWidth);

    /**
     * @brief 渲染单个带范围效果的字符
     * @param rawCanvas 画布
     * @param font 字体
     * @param node 节点
     * @param charInfo 字符信息
     */
    void RenderCharWithRangeEffect(std::shared_ptr<drawing::Canvas> rawCanvas,
                                   drawing::Font* font,
                                   const LOTNode* node,
                                   const CharPositionInfo& charInfo);

/**
     * @brief 处理带文本范围的文本渲染
     * @param rawCanvas 画布
     * @param font 字体对象
     * @param processedLines 处理后的文本行
     * @param node 节点信息
     * @param params 文本渲染参数
     */
    void ProcessTextWithRange(std::shared_ptr<drawing::Canvas> rawCanvas,
                              drawing::Font* font,
                              const std::vector<std::string>& processedLines,
                              const LOTNode* node,
                              const TextRenderParams& params);

    /**
     * @brief 处理不带文本范围的文本渲染
     */
    void ProcessTextWithoutRange(std::shared_ptr<drawing::Canvas> rawCanvas,
                                 drawing::Font* font,
                                 const std::vector<std::string>& processedLines,
                                 const LOTNode* node,
                                 const TextRenderParams& params);

    /**
     * @brief 应用文本范围变换
     */
    void ApplyTextRangeTransform(std::shared_ptr<drawing::Canvas> rawCanvas,
                                 const LOTNode* node,
                                 float factor,
                                 float x,
                                 float y);

    /**
     * @brief 计算HSB调整后的颜色
     * @param rangeParams 颜色范围参数
     * @param hsbParams HSB调整参数
     * @param baseColor 基础颜色信息
     * @param factor 范围因子
     * @return 调整后的颜色值
     */
    uint32_t CalculateHSBAdjustedColor(const ColorRangeParams& rangeParams,
                                       const HSBAdjustParams& hsbParams,
                                       const BaseColorInfo& baseColor,
                                       float factor);

    /**
     * @brief 计算颜色插值
     * @param rangeParams 颜色范围参数
     * @param baseColor 基础颜色信息
     * @param factor 范围因子
     * @return 插值后的颜色值
     */
    uint32_t CalculateColorInterpolation(const ColorRangeParams& rangeParams,
                                         const BaseColorInfo& baseColor,
                                         float factor);
    /**
     * @brief 设置文本范围颜色
     */
    void SetTextRangeColor(std::shared_ptr<drawing::Canvas> rawCanvas,
                           const LOTNode* node,
                           float factor);

    /**
     * @brief 应用文本范围描边
     */
    void ApplyTextRangeStroke(std::shared_ptr<drawing::Canvas> rawCanvas,
                              const drawing::TextBlob* textBlob,
                              const TextPosition& position,
                              const LOTNode* node,
                              float factor);

    /**
     * @brief 计算形状因子
     * @param position 在范围内的位置 (0.0 - 1.0)
     * @param shape 形状类型
     * @return 形状因子
     */
    float CalculateShapeFactor(float position, int shape);

    /**
     * @brief 判断字符是否被随机选中
     * @param node 文本节点
     * @param charIndex 字符索引
     * @param totalChars 总字符数
     * @param targetCount 目标选择数量
     * @return 是否被选中
     */

    bool IsCharacterRandomlySelected(const LOTNode* node, size_t charIndex, size_t totalChars, size_t targetCount);

    /**
     * @brief 计算字符在原始范围中的相对位置
     * @param charIndex 字符索引
     * @param start 范围开始
     * @param end 范围结束
     * @param totalChars 总字符数
     * @return 相对位置 (0.0 - 1.0)
     */
    float CalculateRelativePosition(size_t charIndex, float start, float end, size_t totalChars);

    /**
     * @brief 根据 basedOn 字段计算有效的索引和总数
     * @param node 文本节点
     * @param charIndex 当前字符索引
     * @param processedLines 处理后的文本行
     * @return pair<有效索引, 有效总数>
     */
    std::pair<size_t, size_t> CalculateEffectiveIndex(const LOTNode* node,
                                                      size_t charIndex,
                                                      const std::vector<std::string>& processedLines);

    /**
     * @brief 计算排除空格的字符索引和总数
     * @param charIndex 原始字符索引
     * @param processedLines 处理后的文本行
     * @return pair<排除空格后的索引, 排除空格后的总数>
     */
    std::pair<size_t, size_t> CalculateNonSpaceIndex(size_t charIndex,
                                                     const std::vector<std::string>& processedLines);

    /**
     * @brief 计算基于单词的索引和总数
     * @param charIndex 原始字符索引
     * @param processedLines 处理后的文本行
     * @return pair<单词索引, 单词总数>
     */
    std::pair<size_t, size_t> CalculateWordIndex(size_t charIndex,
                                                 const std::vector<std::string>& processedLines);

    /**
     * @brief 计算基于行的索引和总数
     * @param charIndex 原始字符索引
     * @param processedLines 处理后的文本行
     * @return pair<行索引, 行总数>
     */
    std::pair<size_t, size_t> CalculateLineIndex(size_t charIndex,
                                                 const std::vector<std::string>& processedLines);
    /**
     * @brief RGB转HSB颜色空间
     * @param r 红色分量 (0-255)
     * @param g 绿色分量 (0-255)
     * @param b 蓝色分量 (0-255)
     * @return HSB颜色值
     */
    HSBColor RgbToHsb(uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief HSB转RGB颜色空间
     * @param hsb HSB颜色值
     * @param alpha 透明度 (0-255)
     * @return RGB颜色的uint32_t表示 (ARGB格式)
     */
    uint32_t HsbToRgb(const HSBColor& hsb, uint8_t alpha = 255);
};
} // namespace LottieTurbo

#endif // LOTTIETURBO_LOTTIE_RENDER_NODE_H
