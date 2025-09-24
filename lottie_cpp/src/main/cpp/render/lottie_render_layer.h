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

#ifndef LOTTIETURBO_LOTTIE_RENDER_LAYER_H
#define LOTTIETURBO_LOTTIE_RENDER_LAYER_H

#include "bitmap.h"
#include "brush.h"
#include "cache/generator_key.h"
#include "canvas.h"
#include "color.h"
#include "common/common.h"
#include "path.h"
#include "rect.h"
#include "render/lottie_render_node.h"
#include "rlottiecommon.h"
#include <native_drawing/drawing_types.h>
#include <stack>
#include <string>
#include <unordered_set>

namespace LottieTurbo {

class LottieRenderLayer {
public:
    LottieRenderLayer();
    ~LottieRenderLayer();

    /**
     * 设置尺寸
     * @param width
     * @param height
     */
    void SetSize(float width, float height);

    /**
     * 与Layer::render对应的核心渲染方法
     * @param canvas
     * @param layerNode
     * @param inheritMask
     * @param matteClip
     */
    void RenderLayer(std::shared_ptr<drawing::Canvas> canvas, const LOTLayerNode *layerNode,
                     std::shared_ptr<drawing::Path> inheritMask = nullptr);

    /**
     * 处理蒙版，对应LayerMask::maskRle
     * @param layerNode
     * @param clipRect
     * @return
     */
    std::shared_ptr<drawing::Path> MergeMaskPath(const LOTLayerNode *layerNode,
                                                 std::shared_ptr<drawing::Path> inheritMask = nullptr);

    /**
     * 处理matte层，类似CompLayer::renderMatteLayer
     * @param canvas
     * @param mask
     * @param matteClip
     * @param matteSource
     * @param targetLayer
     */
    std::shared_ptr<drawing::Brush> RenderMatteLayer(std::shared_ptr<drawing::Canvas> canvas,
                                                     std::shared_ptr<drawing::Path> mask,
                                                     const LOTLayerNode *matteSource, const LOTLayerNode *targetLayer);

    /**
     * 处理effect效果
     * @param layerNode
     * @return
     */
    std::shared_ptr<drawing::DrawFilter> RenderEffect(std::shared_ptr<drawing::Canvas> canvas,
                                                      const LOTLayerNode *layerNode);
    /**
     * 处理 blur effect
     * @param canvas
     * @param layerNode
     * @return
     */
    std::shared_ptr<drawing::DrawFilter> HandBlurEffect(std::shared_ptr<drawing::Canvas> canvas,
                                                        const LOTLayerNode *layerNode);

    /**
     * 处理 fill effect
     * @param canvas
     * @param layerNode
     * @return
     */
    std::shared_ptr<drawing::DrawFilter> HandFillEffect(std::shared_ptr<drawing::Canvas> canvas,
                                                        const LOTLayerNode *layerNode);

    /**
     * 处理 tint effect
     * @param canvas
     * @param layerNode
     * @return
     */
    std::shared_ptr<drawing::DrawFilter> HandTintEffect(std::shared_ptr<drawing::Canvas> canvas,
                                                        const LOTLayerNode *layerNode);

    /**
     * 处理 triTone effect
     * @param canvas
     * @param layerNode
     * @return
     */
    std::shared_ptr<drawing::DrawFilter> HandTriToneEffect(std::shared_ptr<drawing::Canvas> canvas,
                                                           const LOTLayerNode *layerNode);

    /**
     * 处理 stroke effect
     * @param canvas
     * @param layerNode
     * @return
     */
    std::shared_ptr<drawing::DrawFilter> HandStrokeEffect(std::shared_ptr<drawing::Canvas> canvas,
                                                          const LOTLayerNode *layerNode);

    /**
     * @brief 打印渲染信息
     */
    void PrintRenderInfo();
private:
    /**
     * @brief 设置图层画布的状态
     * @param canvas 目标画布对象
     * @param layerNode 待处理的图层节点
     */
    void SetupLayerCanvas(std::shared_ptr<drawing::Canvas> canvas, const LOTLayerNode *layerNode);

    /**
     * @brief 处理图层的蒙版
     * @param canvas 目标画布对象
     * @param layerNode 待处理的图层节点
     * @param inheritMask 继承的蒙版路径
     * @return 处理后的蒙版路径
     */
    std::shared_ptr<drawing::Path> ProcessMask(std::shared_ptr<drawing::Canvas> canvas, const LOTLayerNode *layerNode,
                                               std::shared_ptr<drawing::Path> inheritMask);

    /**
     * @brief 处理图层的子节点
     * @param canvas 目标画布对象
     * @param layerNode 待处理的图层节点
     * @param mask 当前的蒙版路径
     */
    void ProcessChildNodes(std::shared_ptr<drawing::Canvas> canvas, const LOTLayerNode *layerNode,
                           std::shared_ptr<drawing::Path> mask);

    /**
     * @brief 处理图层间的遮罩关系
     * @param canvas 目标画布对象
     * @param layerNode 待处理的图层节点
     * @param mask 当前的蒙版路径
     */
    void ProcessMatteRelationships(std::shared_ptr<drawing::Canvas> canvas, const LOTLayerNode *layerNode,
                                   std::shared_ptr<drawing::Path> mask);

    /**
     * @brief 构建图层间的遮罩关系
     * @param layerNode 待处理的图层节点
     * @param indToLayerMap 索引到图层的映射表
     * @param matteSourceStack 遮罩源图层栈
     * @param targetLayerStack 目标图层栈
     * @return 遮罩关系对的vector
     */
    std::vector<std::pair<const LOTLayerNode *, const LOTLayerNode *>>
    BuildMatteRelationships(const LOTLayerNode *layerNode,
                            std::unordered_map<int, const LOTLayerNode *> &indToLayerMap);
    
    /**
     * @brief 查找与目标层匹配的遮罩源
     * @param layer 需要查找遮罩源的目标图层
     * @param mattePairs 遮罩关系对列表
     * @return 匹配的遮罩源图层指针，如果未找到则返回nullptr
     */
    const LOTLayerNode* FindMatchingMatteSource(
        const LOTLayerNode* layer,
        const std::vector<std::pair<const LOTLayerNode*, const LOTLayerNode*>>& mattePairs);

    /**
     * @brief 清理画布状态
     * @param canvas 目标画布对象
     * @param layerNode 待处理的图层节点
     * @param mask 当前的蒙版路径
     */
    void CleanupLayerCanvas(std::shared_ptr<drawing::Canvas> canvas, const LOTLayerNode *layerNode,
                            std::shared_ptr<drawing::Path> mask);

    /**
     * @brief 初始化遮罩路径，从继承的遮罩中复制内容
     * @param inheritMask 继承的遮罩
     * @return 初始化后的遮罩路径
     */
    std::shared_ptr<drawing::Path> InitializeMaskPath(std::shared_ptr<drawing::Path> inheritMask);

    /**
     * @brief 处理常规遮罩
     * @param resultMask 结果遮罩
     * @param maskPath 当前处理的遮罩路径
     * @param mask 遮罩对象
     */
    void ProcessRegularMask(std::shared_ptr<drawing::Path>& resultMask,
                            drawing::Path maskPath,
                            const LOTMask& mask);

    /**
     * @brief 处理反转遮罩
     * @param resultMask 结果遮罩
     * @param maskPath 当前处理的遮罩路径
     * @param mask 遮罩对象
     */
    void ProcessInvertedMask(std::shared_ptr<drawing::Path>& resultMask,
                             drawing::Path maskPath,
                             const LOTMask& mask);

private:
    // 缓存离屏渲染位图
    float currentWidth_{0};
    float currentHeight_{0};
    bool maskClip_{true};
    std::unordered_map<std::string, OH_Drawing_Bitmap *> offscreenCache_;
    std::shared_ptr<LottieRenderNode> lottieRenderNode_{nullptr};
    std::shared_ptr<drawing::Canvas> canvas_{nullptr};
    bool isNotClipPathOfCanvas_{false}; // 判断ef属性下stroke效果，canvas是否需要显示原背景色。
    std::shared_ptr<drawing::DrawFilter> effectFilter_{nullptr};
};

} // namespace LottieTurbo

#endif // LOTTIETURBO_LOTTIE_RENDER_LAYER_H
