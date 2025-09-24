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

#ifndef LOTTIETURBO_BITMAP_H
#define LOTTIETURBO_BITMAP_H

#include <memory>
#include <native_drawing/drawing_bitmap.h>
#include <native_drawing/drawing_types.h>

namespace LottieTurbo::drawing {

class Bitmap final {
    // 使用智能指针管理原生bitmap资源
    using UniqueNativeBitmap = std::unique_ptr<OH_Drawing_Bitmap, decltype(&OH_Drawing_BitmapDestroy)>;

public:
    // 构造和析构函数
    Bitmap();
    ~Bitmap() noexcept = default;

    // 禁用拷贝
    Bitmap(const Bitmap &) = delete;
    Bitmap &operator=(const Bitmap &) = delete;

    // 启用移动
    Bitmap(Bitmap &&) = default;
    Bitmap &operator=(Bitmap &&) = default;

    // 从像素创建位图
    static Bitmap CreateFromPixels(OH_Drawing_Image_Info *imageInfo, void *pixels, uint32_t rowBytes);

    // 构建位图
    void Build(uint32_t width, uint32_t height, const OH_Drawing_BitmapFormat &format);

    // 获取属性
    uint32_t GetWidth() const;
    uint32_t GetHeight() const;
    OH_Drawing_ColorFormat GetColorFormat() const;
    OH_Drawing_AlphaFormat GetAlphaFormat() const;
    void *GetPixels() const;

    // 获取图像信息
    void GetImageInfo(OH_Drawing_Image_Info *imageInfo) const;

    // 读取像素
    bool ReadPixels(const OH_Drawing_Image_Info *dstInfo, void *dstPixels, size_t dstRowBytes, int32_t srcX,
                    int32_t srcY) const;

    // 获取原生bitmap对象
    OH_Drawing_Bitmap *get() const
    {
        return bitmap_.get();
    }

private:
    UniqueNativeBitmap bitmap_;
};

} // namespace LottieTurbo::drawing

#endif