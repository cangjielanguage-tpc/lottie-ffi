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

#ifndef LOTTIETURBO_LOTTIE_RENDER_H
#define LOTTIETURBO_LOTTIE_RENDER_H

#include "color.h"
#include "render/lottie_render_layer.h"
#include "rlottiecommon.h"
#include <string>

namespace LottieTurbo {

struct CalculatePosition {
    bool keepAspectRatio = true;
    int width = 0;
    int height = 0;
    int x = 0;
    int y = 0;
};

class LottieRender {
public:
    LottieRender();
    ~LottieRender();

    /**
     *  渲染 LOTLayerNode
     * @param id
     * @param width
     * @param height
     * @param rootNode
     * @param x
     * @param y
     */
    void RenderTree(const std::string xcomponentId, const LOTLayerNode *rootNode, CalculatePosition *calculatePosition);

    /**
     *  渲染 LOTLayerNode 为图像数据
     * @param rootNode 解析后的帧数据
     * @param width 图片宽度
     * @param height 图片高度
     * @param bitmap 图片数据
     */
    bool RenderToImage(const LOTLayerNode *rootNode, float width, float height, void *bitmap);

    /**
     * 处理canvas
     * @param canvas
     * @param calculatePosition
     */
    void HandleCanvas(std::shared_ptr<drawing::Canvas> canvas, CalculatePosition *calculatePosition);

    void destroy(const std::string xcomponentId, CalculatePosition *calculatePosition);
    

private:
    std::shared_ptr<LottieRenderLayer> lottieRenderLayer_{nullptr};
    std::shared_ptr<drawing::Canvas> canvas_{nullptr};
    OH_Drawing_GpuContext *gpuContext_{nullptr};
    OH_Drawing_Surface *surface_{nullptr};
};

} // namespace LottieTurbo

#endif // LOTTIETURBO_LOTTIE_RENDER_H