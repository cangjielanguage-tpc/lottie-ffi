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

#include "render/lottie_render_layer.h"
#include "common/lottile_utils.h"
#include "common/number_constant.h"
#include <stack>
#include <unordered_set>

namespace LottieTurbo {

LottieRenderLayer::LottieRenderLayer()
{
    lottieRenderNode_ = std::make_shared<LottieRenderNode>();
    canvas_ = std::make_shared<drawing::Canvas>();
}

LottieRenderLayer::~LottieRenderLayer()
{
    if (effectFilter_) {
        effectFilter_->Destroy();
        effectFilter_ = nullptr;
    }
}

void LottieRenderLayer::SetSize(float width, float height)
{
    currentWidth_ = width;
    currentHeight_ = height;
}

void LottieRenderLayer::RenderLayer(std::shared_ptr<drawing::Canvas> canvas, const LOTLayerNode *layerNode,
                                    std::shared_ptr<drawing::Path> inheritMask)
{
    if (!layerNode || !canvas) {
        return;
    }

    // 处理可见性
    if (!layerNode->mVisible) {
        return;
    }

    if (layerNode->mAlpha == 0) {
        return;
    }

    // 设置画布并处理透明度
    SetupLayerCanvas(canvas, layerNode);

    // 应用效果
    effectFilter_ = RenderEffect(canvas, layerNode);

    // 处理蒙版
    auto mask = ProcessMask(canvas, layerNode, inheritMask);

    // 处理子节点
    ProcessChildNodes(canvas, layerNode, mask);

    if (effectFilter_) {
        canvas->DetachPen();
    }

    // 处理子图层之间的遮罩关系
    ProcessMatteRelationships(canvas, layerNode, mask);

    // 清理画布状态
    CleanupLayerCanvas(canvas, layerNode, mask);
}

void LottieRenderLayer::SetupLayerCanvas(std::shared_ptr<drawing::Canvas> canvas, const LOTLayerNode *layerNode)
{
    // 保存画布状态
    canvas->Save();

    // 应用透明度
    if (layerNode->mAlpha < MAX_8BIT_COLOR) {
        float alpha = layerNode->mAlpha / MAX_8BIT_COLOR_FLOAT;
        drawing::Brush alphaBrush;
        alphaBrush.SetColor(drawing::Color::SetArgb(static_cast<uint8_t>(alpha * MAX_8BIT_COLOR), MAX_8BIT_COLOR,
                                                    MAX_8BIT_COLOR, MAX_8BIT_COLOR));
        canvas->SaveLayer(nullptr, alphaBrush.get());
        lottieRenderNode_->renderInfo_.savelayer++;
    }

    if (layerNode->mComplexContent) {
        float alpha = layerNode->mCombinedAlpha;
        drawing::Brush alphaBrush;
        alphaBrush.SetColor(drawing::Color::SetArgb(static_cast<uint8_t>(alpha * MAX_8BIT_COLOR), MAX_8BIT_COLOR,
                                                    MAX_8BIT_COLOR, MAX_8BIT_COLOR));
        canvas->SaveLayer(nullptr, alphaBrush.get());
        lottieRenderNode_->renderInfo_.savelayer++;
    }
}

std::shared_ptr<drawing::Path> LottieRenderLayer::ProcessMask(std::shared_ptr<drawing::Canvas> canvas,
                                                              const LOTLayerNode *layerNode,
                                                              std::shared_ptr<drawing::Path> inheritMask)
{
    // 处理蒙版
    std::shared_ptr<drawing::Path> mask = nullptr;
    if (layerNode->mMaskList.size > 0) {
        if (inheritMask) {
            mask = MergeMaskPath(layerNode, inheritMask);
        } else {
            mask = MergeMaskPath(layerNode);
        }

        if (mask && !isNotClipPathOfCanvas_ && maskClip_) {
            // 应用蒙版裁剪
            canvas->Save();
            canvas->ClipPath(mask->get(), INTERSECT, true);
            lottieRenderNode_->renderInfo_.mask++;
        } else if (maskClip_ == false) {
            maskClip_ = true;
        }
    } else {
        mask = inheritMask;
        if (mask && !isNotClipPathOfCanvas_) {
            canvas->Save();
            canvas->ClipPath(mask->get(), INTERSECT, true);
            lottieRenderNode_->renderInfo_.mask++;
        }
    }

    return mask;
}

void LottieRenderLayer::ProcessChildNodes(std::shared_ptr<drawing::Canvas> canvas, const LOTLayerNode *layerNode,
                                          std::shared_ptr<drawing::Path> mask)
{
    lottieRenderNode_->SetCurrentTextLayerNode(layerNode);
    // 处理子节点
    for (size_t i = 0; i < layerNode->mNodeList.size; i++) {
        if (layerNode->mNodeList.ptr[i]) {
            lottieRenderNode_->ProcessNode(canvas, layerNode->mNodeList.ptr[i], mask);
        }
    }
    lottieRenderNode_->SetCurrentTextLayerNode(nullptr);
}

void LottieRenderLayer::ProcessMatteRelationships(std::shared_ptr<drawing::Canvas> canvas,
                                                  const LOTLayerNode *layerNode, std::shared_ptr<drawing::Path> mask)
{
    // 创建索引映射表和栈
    std::unordered_map<int, const LOTLayerNode *> indToLayerMap;

    // 构建图层索引和遮罩关系
    auto mattePairs = BuildMatteRelationships(layerNode, indToLayerMap);

    std::unordered_map<int, int> matteSourceRefCount;
    // 统计每个遮罩源被引用的次数
    for (const auto &pair : mattePairs) {
        if (pair.second->tp > 0) {
            matteSourceRefCount[pair.second->mId] = pair.first->mId;
        }
    }

    for (size_t i = 0; i < layerNode->mLayerList.size; i++) {
        const LOTLayerNode *layer = layerNode->mLayerList.ptr[i];
        if (!layer || !layer->mVisible) {
            continue;
        }
        // 查找该图层是否使用了遮罩
        auto it = matteSourceRefCount.find(layer->mId);
        if (it != matteSourceRefCount.end()) {
            auto targetLayer = indToLayerMap.find(it->second);
            auto matteLayer = indToLayerMap.find(it->first);
            if (targetLayer != indToLayerMap.end() && matteLayer != indToLayerMap.end()) {
                RenderMatteLayer(canvas, mask, targetLayer->second, matteLayer->second);
            }
        } else if (layer->mMatte != MatteNone && layer->tp <= 0) {
            // 查找对应的遮罩源
            const LOTLayerNode *matteSource = FindMatchingMatteSource(layer, mattePairs);

            if (matteSource && matteSource->mVisible) {
                RenderMatteLayer(canvas, mask, matteSource, layer);
            }
        } else if (!layer->td && layer->mMatte == MatteNone) {
            // 处理普通图层（不是遮罩源也不是被遮罩层）
            RenderLayer(canvas, layer, mask);
        }
    }
}
std::vector<std::pair<const LOTLayerNode *, const LOTLayerNode *>>
LottieRenderLayer::BuildMatteRelationships(const LOTLayerNode *layerNode,
                                           std::unordered_map<int, const LOTLayerNode *> &indToLayerMap)
{
    std::stack<const LOTLayerNode *> matteSourceStack; // 遮罩源栈
    std::stack<const LOTLayerNode *> targetLayerStack; // 目标层栈

    // 第一次遍历 - 收集所有图层信息并建立索引映射
    for (size_t i = 0; i < layerNode->mLayerList.size; i++) {
        const LOTLayerNode *layer = layerNode->mLayerList.ptr[i];
        if (!layer) {
            continue;
        }

        // 保存到索引映射
        indToLayerMap[layer->mId] = layer;

        // 将遮罩源和目标分别入栈
        if (layer->td) {
            matteSourceStack.push(layer);
        }

        if (layer->mMatte != MatteNone) {
            targetLayerStack.push(layer);
        }
    }

    // 匹配遮罩源和目标图层
    std::vector<std::pair<const LOTLayerNode *, const LOTLayerNode *>> matteLayerPairs;

    while (!targetLayerStack.empty()) {
        auto targetLayer = targetLayerStack.top();
        targetLayerStack.pop();

        const LOTLayerNode *matteSource = nullptr;

        // 优先使用tp指定的遮罩源
        if (targetLayer->tp > 0 && indToLayerMap.find(targetLayer->tp) != indToLayerMap.end()) {
            matteSource = indToLayerMap[targetLayer->tp];
        } else if (!matteSourceStack.empty()) {
            // 如果没有指定tp，从栈中取得合适的遮罩源
            matteSource = matteSourceStack.top();
            matteSourceStack.pop();
        }

        if (matteSource && matteSource->mVisible) {
            matteLayerPairs.push_back({matteSource, targetLayer});
        }
    }

    return matteLayerPairs;
}

const LOTLayerNode *LottieRenderLayer::FindMatchingMatteSource(
    const LOTLayerNode *layer, const std::vector<std::pair<const LOTLayerNode *, const LOTLayerNode *>> &mattePairs)
{
    for (const auto &pair : mattePairs) {
        if (pair.second == layer) {
            return pair.first;
        }
    }
    return nullptr;
}

void LottieRenderLayer::CleanupLayerCanvas(std::shared_ptr<drawing::Canvas> canvas, const LOTLayerNode *layerNode,
                                           std::shared_ptr<drawing::Path> mask)
{
    size_t alpha = 255;
    // 恢复蒙版裁剪
    if (mask) {
        canvas->Restore();
    }

    // 恢复透明度设置
    if (layerNode->mAlpha < alpha) {
        canvas->Restore();
    }

    if (layerNode->mComplexContent) {
        canvas->Restore();
    }

    // 清空ef属性设置的效果
    lottieRenderNode_->ClearEffectOfEf();

    // 恢复画布状态
    canvas->Restore();
}

std::shared_ptr<drawing::Path> LottieRenderLayer::InitializeMaskPath(std::shared_ptr<drawing::Path> inheritMask)
{
    std::shared_ptr<drawing::Path> resultMask = std::make_shared<drawing::Path>();
    if (inheritMask) {
        resultMask->AddPath(*inheritMask);
    }
    return resultMask;
}

void LottieRenderLayer::ProcessRegularMask(std::shared_ptr<drawing::Path> &resultMask, drawing::Path maskPath,
                                           const LOTMask &mask)
{
    switch (mask.mMode) {
        case MaskNone:
            resultMask->Reset();
            maskClip_ = false;
            break;
        case MaskAdd:
            resultMask->AddPath(maskPath);
            break;
        case MaskSubstract:
            resultMask->Difference(maskPath);
            break;
        case MaskIntersect:
            resultMask->Intersect(maskPath);
            break;
        case MaskDifference:
            resultMask->Xor(maskPath);
            break;
        default:
            DRAWING_LOGE("Unknown mask mode: %{public}d", mask.mMode);
            break;
    }
}

void LottieRenderLayer::ProcessInvertedMask(std::shared_ptr<drawing::Path> &resultMask, drawing::Path maskPath,
                                            const LOTMask &mask)
{
    // 创建一个覆盖整个区域的路径
    drawing::Path fullPath;
    fullPath.AddRect(0, 0, currentWidth_, currentHeight_, PATH_DIRECTION_CW);

    switch (mask.mMode) {
        case MaskNone:
            resultMask->Reset();
            maskClip_ = false;
            break;
        case MaskAdd:
            // 对于反转的Add 需要从全区域中减去遮罩路径
            fullPath.Difference(maskPath);
            resultMask->AddPath(fullPath);
            break;
        case MaskSubstract:
            // 对于反转的Substract，需要添加遮罩路径
            resultMask->AddPath(maskPath);
            break;
        case MaskIntersect:
            // 对于反转的Intersect，需要与遮罩路径的补集取交集
            fullPath.Difference(maskPath);
            resultMask->Intersect(fullPath);
            break;
        case MaskDifference:
            // Xor操作在反转时保持不变
            resultMask->Xor(maskPath);
            break;
        default:
            DRAWING_LOGE("Unknown inverted mask mode: %{public}d", mask.mMode);
            break;
    }
}

std::shared_ptr<drawing::Path> LottieRenderLayer::MergeMaskPath(const LOTLayerNode *layerNode,
                                                                std::shared_ptr<drawing::Path> inheritMask)
{
    if (!layerNode || layerNode->mMaskList.size == 0) {
        return inheritMask; // 直接返回传入的蒙版
    }

    // 初始化结果蒙版
    std::shared_ptr<drawing::Path> resultMask = InitializeMaskPath(inheritMask);

    // 处理每个遮罩
    drawing::Path maskPath;
    for (size_t i = 0; i < layerNode->mMaskList.size; i++) {
        const LOTMask &mask = layerNode->mMaskList.ptr[i];
        maskPath.Reset(); // 重置临时路径
        lottieRenderNode_->ConvertLottiePathToDrawingPath(mask.mPath.ptPtr, mask.mPath.ptCount, mask.mPath.elmPtr,
                                                          mask.mPath.elmCount, maskPath);

        // 根据遮罩类型选择处理方法
        if (!mask.mInv) {
            ProcessRegularMask(resultMask, maskPath, mask);
        } else {
            ProcessInvertedMask(resultMask, maskPath, mask);
        }
    }

    return resultMask;
}

std::shared_ptr<drawing::Brush> LottieRenderLayer::RenderMatteLayer(std::shared_ptr<drawing::Canvas> canvas,
                                                                    std::shared_ptr<drawing::Path> mask,
                                                                    const LOTLayerNode *matteSource,
                                                                    const LOTLayerNode *targetLayer)
{
    std::shared_ptr<drawing::Brush> brush = std::make_shared<drawing::Brush>();
    if (!canvas || !matteSource || !targetLayer)
        return brush;

    canvas->SaveLayer(nullptr, nullptr);
    lottieRenderNode_->renderInfo_.savelayer++;
    RenderLayer(canvas, targetLayer, mask);

    // 配置混合模式
    constexpr uint8_t colorValueLimits = 255;
    brush->SetColor(drawing::Color::SetArgb(matteSource->mAlpha, colorValueLimits, colorValueLimits, colorValueLimits));

    switch (targetLayer->mMatte) {
        case MatteAlpha:
            brush->SetBlendMode(BLEND_MODE_DST_IN);
            break;
        case MatteAlphaInv:
            brush->SetBlendMode(BLEND_MODE_DST_OUT);
            break;
        case MatteLuma:
            break;
        case MatteLumaInv:
            break;
        default:
            brush->SetBlendMode(BLEND_MODE_DST_IN);
    }

    // 应用遮罩混合操作
    canvas->SaveLayer(nullptr, brush->get());
    lottieRenderNode_->renderInfo_.savelayer++;
    RenderLayer(canvas, matteSource, mask);
    canvas->Restore();
    canvas->Restore();
    return brush;
}

std::shared_ptr<drawing::DrawFilter> LottieRenderLayer::RenderEffect(std::shared_ptr<drawing::Canvas> canvas,
                                                                     const LOTLayerNode *layerNode)
{
    std::shared_ptr<drawing::DrawFilter> effectFilter{nullptr};
    if (layerNode->mEffectList.ptr && layerNode->mEffectList.size > 0) {
        switch (layerNode->mEffectList.ptr->mEffectType) {
            case LOTEffectType::Effect_Gaussian:
                effectFilter = HandBlurEffect(canvas, layerNode);
                lottieRenderNode_->renderInfo_.blurEff++;
                break;
            case LOTEffectType::Effect_Fill:
                effectFilter = HandFillEffect(canvas, layerNode);
                lottieRenderNode_->renderInfo_.fillEff++;
                break;
            case LOTEffectType::Effect_Tint:
                effectFilter = HandTintEffect(canvas, layerNode);
                lottieRenderNode_->renderInfo_.tintEff++;
                break;
            case LOTEffectType::Effect_Tritone:
                effectFilter = HandTriToneEffect(canvas, layerNode);
                lottieRenderNode_->renderInfo_.tritoneEff++;
                break;
            case LOTEffectType::Effect_Stroke:
                effectFilter = HandStrokeEffect(canvas, layerNode);
                lottieRenderNode_->renderInfo_.strokeEff++;
                break;
            default:
                break;
        }
    }
    return effectFilter;
}

std::shared_ptr<drawing::DrawFilter> LottieRenderLayer::HandBlurEffect(std::shared_ptr<drawing::Canvas> canvas,
                                                                       const LOTLayerNode *layerNode)
{
    constexpr size_t arrayMin = 2;
    std::shared_ptr<drawing::DrawFilter> effectFilter{nullptr};
    for (size_t i = 0; i < layerNode->mEffectList.size; i++) {
        auto arrays = layerNode->mEffectList.ptr[i].mEffectValueArray;
        if (arrays.size() >= arrayMin) {
            auto value = px2vp(arrays[0].k);
            auto direction = static_cast<int>(arrays[1].k);
            if (direction == EFFECT_HORIZONTAL) {
                effectFilter = lottieRenderNode_->SetBluer(canvas, value, 0.0);
            } else if (direction == EFFECT_VERTICAL) {
                effectFilter = lottieRenderNode_->SetBluer(canvas, 0.0, value);
            } else {
                effectFilter = lottieRenderNode_->SetBluer(canvas, value, value);
            }
        }
        break;
    }
    return effectFilter;
}
std::shared_ptr<drawing::DrawFilter> LottieRenderLayer::HandFillEffect(std::shared_ptr<drawing::Canvas> canvas,
                                                                       const LOTLayerNode *layerNode)
{
    for (size_t i = 0; i < layerNode->mEffectList.size; i++) {
        auto arrays = layerNode->mEffectList.ptr[i].mEffectValueArray;
        /**
         * https://lottiefiles.github.io/lottie-docs/effects/#fill-effect
         * color属性数组坐标index为2，opacity数组坐标index位于6
         */
        if (arrays.size() >= NUM_7 && arrays[NUM_2].mEffectValueType == EFFECT_VALUE_Color &&
            arrays[NUM_2].k_array.size() >= NUM_3) {
            auto percent = std::make_shared<ColorPercent>();
            percent.get()->r = arrays[NUM_2].k_array[NUM_0];
            percent.get()->g = arrays[NUM_2].k_array[NUM_1];
            percent.get()->b = arrays[NUM_2].k_array[NUM_2];
            percent.get()->opacity = arrays[NUM_6].k;
            lottieRenderNode_->SetEffectColorPercent(percent);
        }
    }
    return nullptr;
}

std::shared_ptr<drawing::DrawFilter> LottieRenderLayer::HandTintEffect(std::shared_ptr<drawing::Canvas> canvas,
                                                                       const LOTLayerNode *layerNode)
{
    /**
     * https://lottiefiles.github.io/lottie-docs/effects/#tint-effect
     */
    for (size_t i = 0; i < layerNode->mEffectList.size; i++) {
        auto arrays = layerNode->mEffectList.ptr[i].mEffectValueArray;
        if (arrays.size() >= NUM_3) {
            auto colorBlack = std::move(arrays[NUM_0].k_array);
            auto colorWhite = std::move(arrays[NUM_1].k_array);
            auto opacity = arrays[NUM_2].k;
            lottieRenderNode_->SetTintColor(colorBlack, colorWhite, opacity);
        }
    }
    return nullptr;
}

std::shared_ptr<drawing::DrawFilter> LottieRenderLayer::HandTriToneEffect(std::shared_ptr<drawing::Canvas> canvas,
                                                                          const LOTLayerNode *layerNode)
{
    for (size_t i = 0; i < layerNode->mEffectList.size; i++) {
        /**
         * https://lottiefiles.github.io/lottie-docs/effects/#tritone-effect
         */
        auto arrays = layerNode->mEffectList.ptr[i].mEffectValueArray;
        if (arrays.size() >= NUM_3) {
            std::vector<std::vector<float>> result;
            auto shadowColor = std::move(arrays[NUM_0].k_array);
            auto midToneColor = std::move(arrays[NUM_1].k_array);
            auto highlightColor = std::move(arrays[NUM_2].k_array);
            result.push_back(shadowColor);
            result.push_back(midToneColor);
            result.push_back(highlightColor);
            lottieRenderNode_->SetTriToneColor(result);
        }
    }
    return nullptr;
}

std::shared_ptr<drawing::DrawFilter> LottieRenderLayer::HandStrokeEffect(std::shared_ptr<drawing::Canvas> canvas,
                                                                         const LOTLayerNode *layerNode)
{
    for (size_t i = 0; i < layerNode->mEffectList.size; i++) {
        auto arrays = layerNode->mEffectList.ptr[i].mEffectValueArray;
        if (arrays.size() >= NUM_11) {
            auto props = std::make_shared<StrokeProps>();
            props.get()->isAllMask = static_cast<int>(arrays[NUM_1].k);
            props.get()->width = arrays[NUM_4].k;
            props.get()->opacity = arrays[NUM_6].k;
            props.get()->lineSpace = arrays[NUM_9].k;
            props.get()->showType = static_cast<int>(arrays[NUM_10].k);
            props.get()->colors = arrays[NUM_3].k_array;
            lottieRenderNode_->SetEffectStrokeProps(props);
            isNotClipPathOfCanvas_ = static_cast<int>(arrays[NUM_10].k) == StrokeShowType::DISPLAY_BACKGROUND;
        }
    }
    return nullptr;
}

void LottieRenderLayer::PrintRenderInfo()
{
    DRAWING_LOGD("Lottie RenderInfo::Number of Render Path:%{public}zd", lottieRenderNode_->renderInfo_.path);
    DRAWING_LOGD("Lottie RenderInfo::Number of Render Text:%{public}zd", lottieRenderNode_->renderInfo_.text);
    DRAWING_LOGD("Lottie RenderInfo::Number of Render Image:%{public}zd", lottieRenderNode_->renderInfo_.image);
    DRAWING_LOGD("Lottie RenderInfo::Number of Render SaveLayer:%{public}zd", lottieRenderNode_->renderInfo_.savelayer);
    DRAWING_LOGD("Lottie RenderInfo::Number of Render Mask:%{public}zd", lottieRenderNode_->renderInfo_.mask);
    DRAWING_LOGD("Lottie RenderInfo::Number of Render StrokeEff:%{public}zd", lottieRenderNode_->renderInfo_.strokeEff);
    DRAWING_LOGD("Lottie RenderInfo::Number of Render TritoneEff:%{public}zd",
                 lottieRenderNode_->renderInfo_.tritoneEff);
    DRAWING_LOGD("Lottie RenderInfo::Number of Render TintEff:%{public}zd", lottieRenderNode_->renderInfo_.tintEff);
    DRAWING_LOGD("Lottie RenderInfo::Number of Render FillEff:%{public}zd", lottieRenderNode_->renderInfo_.fillEff);
    DRAWING_LOGD("Lottie RenderInfo::Number of Render BlurEff:%{public}zd", lottieRenderNode_->renderInfo_.blurEff);
    lottieRenderNode_->ClearRenderInfo();
}
} // namespace LottieTurbo
