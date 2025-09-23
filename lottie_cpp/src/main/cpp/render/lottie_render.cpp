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

#include "render/lottie_render.h"
#include "canvas.h"
#include "common/common.h"
#include "native_drawing/drawing_gpu_context.h"
#include "native_drawing/drawing_surface.h"
#include "node/lottie_manager_map.h"
#include "task_pool/task_pool.h"
#include <cstdint>
#include <native_buffer/native_buffer.h>

namespace LottieTurbo {

LottieRender::LottieRender()
{
    lottieRenderLayer_ = std::make_shared<LottieRenderLayer>();
    canvas_ = std::make_shared<drawing::Canvas>();
}

LottieRender::~LottieRender()
{
    if (gpuContext_ != nullptr) {
        auto func = [](OH_Drawing_GpuContext *gpuContext) {
            if (gpuContext != nullptr) {
                OH_Drawing_GpuContextDestroy(gpuContext);
                gpuContext = nullptr;
            }
        };
        TaskPoolLocal::GetInstance().PushTask(func, gpuContext_);
    }
}

void LottieRender::RenderTree(const std::string xcomponentId, const LOTLayerNode *rootNode,
                              CalculatePosition *calculatePosition)
{
    if (!rootNode || calculatePosition->width <= 0 || calculatePosition->height <= 0) {
        DRAWING_LOGE("Invalid parameters for RenderTree");
        return;
    }
    auto window = LottieManagerMap::getInstance().GetWindowById(xcomponentId);
    if (!window) {
        DRAWING_LOGE("Failed to get window for xcomponentId: %s", xcomponentId.c_str());
        return;
    }
    if (gpuContext_ == nullptr) {
        gpuContext_ = OH_Drawing_GpuContextCreate();
    }
    int32_t width = 0;
    int32_t height = 0;
    lottieRenderLayer_->SetSize(calculatePosition->width, calculatePosition->height);
    LottieManagerMap::getInstance().GetRectById(xcomponentId, width, height);
    if (width == 0 || height == 0) {
        return;
    }
    OH_Drawing_Image_Info imageinfo = {static_cast<int32_t>(width), static_cast<int32_t>(height),
                                       COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL};
    surface_ = OH_Drawing_SurfaceCreateOnScreen(gpuContext_, imageinfo, window);
    canvas_ = std::make_shared<drawing::Canvas>(OH_Drawing_SurfaceGetCanvas(surface_));
    this->HandleCanvas(canvas_, calculatePosition);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieRender::RenderTree】----------【ee】");
    // 处理根层节点
    lottieRenderLayer_->RenderLayer(canvas_, rootNode);
    lottieRenderLayer_->PrintRenderInfo();
    OH_Drawing_SurfaceFlush(surface_);
    OH_LOG_Print(LOG_APP, LOG_ERROR, LOG_DOMAIN, "LottieTur播放动画", "【LottieRender::RenderTree】----------【ee】-1");
    OH_Drawing_SurfaceDestroy(surface_);
}

bool LottieRender::RenderToImage(const LOTLayerNode *rootNode, float width, float height, void *bitmap)
{
    if (rootNode == nullptr || width <= 0 || height <= 0) {
        DRAWING_LOGE("LottieRender::RenderToImage, Input Error.");
        return false;
    }
    OH_Drawing_Image_Info imageinfo = {static_cast<int32_t>(width), static_cast<int32_t>(height),
                                       COLOR_FORMAT_RGBA_8888, ALPHA_FORMAT_PREMUL};
    OH_Drawing_GpuContext *offScreenGpuContext_ = OH_Drawing_GpuContextCreate();
    OH_Drawing_Surface *offScreenSurface_ =
        OH_Drawing_SurfaceCreateFromGpuContext(offScreenGpuContext_, false, imageinfo);
    if (offScreenSurface_ == nullptr) {
        DRAWING_LOGE("LottieRender::RenderToImage, Surface Null");
        return false;
    }
    auto offScreenCanvas_ = std::make_shared<drawing::Canvas>(OH_Drawing_SurfaceGetCanvas(offScreenSurface_));
    if (offScreenCanvas_ == nullptr) {
        DRAWING_LOGE("LottieRender::RenderToImage, Canvas Null");
        return false;
    }
    lottieRenderLayer_->RenderLayer(offScreenCanvas_, rootNode);
    bool ret = offScreenCanvas_->ReadPixels(&imageinfo, bitmap, width * RGBANUM, 0, 0);
    if (offScreenSurface_ != nullptr) {
        OH_Drawing_SurfaceDestroy(offScreenSurface_);
        offScreenSurface_ = nullptr;
    }
    if (offScreenGpuContext_ != nullptr) {
        OH_Drawing_GpuContextDestroy(offScreenGpuContext_);
        offScreenGpuContext_ = nullptr;
    }
    return ret;
}

void LottieRender::HandleCanvas(std::shared_ptr<drawing::Canvas> canvas, CalculatePosition *calculatePosition)
{
    if (canvas == nullptr) {
        return;
    }
    // 使用透明清除画布内容
    canvas->Clear(drawing::Color::SetArgb(0, 0, 0, 0));
    // 应用偏移
    if (calculatePosition->x != 0 || calculatePosition->y != 0) {
        canvas->Translate(static_cast<float>(calculatePosition->x), static_cast<float>(calculatePosition->y));
    }
}

} // namespace LottieTurbo