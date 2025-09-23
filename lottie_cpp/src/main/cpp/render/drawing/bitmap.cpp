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

#include "bitmap.h"

namespace LottieTurbo::drawing {

Bitmap::Bitmap() : bitmap_(UniqueNativeBitmap(OH_Drawing_BitmapCreate(), &OH_Drawing_BitmapDestroy))
{
}

Bitmap Bitmap::CreateFromPixels(OH_Drawing_Image_Info *imageInfo, void *pixels, uint32_t rowBytes)
{
    Bitmap bitmap;
    bitmap.bitmap_.reset(OH_Drawing_BitmapCreateFromPixels(imageInfo, pixels, rowBytes));
    return bitmap;
}

void Bitmap::Build(uint32_t width, uint32_t height, const OH_Drawing_BitmapFormat &format)
{
    OH_Drawing_BitmapBuild(bitmap_.get(), width, height, &format);
}

uint32_t Bitmap::GetWidth() const
{
    return OH_Drawing_BitmapGetWidth(bitmap_.get());
}

uint32_t Bitmap::GetHeight() const
{
    return OH_Drawing_BitmapGetHeight(bitmap_.get());
}

OH_Drawing_ColorFormat Bitmap::GetColorFormat() const
{
    return OH_Drawing_BitmapGetColorFormat(bitmap_.get());
}

OH_Drawing_AlphaFormat Bitmap::GetAlphaFormat() const
{
    return OH_Drawing_BitmapGetAlphaFormat(bitmap_.get());
}

void *Bitmap::GetPixels() const
{
    return OH_Drawing_BitmapGetPixels(bitmap_.get());
}

void Bitmap::GetImageInfo(OH_Drawing_Image_Info *imageInfo) const
{
    OH_Drawing_BitmapGetImageInfo(bitmap_.get(), imageInfo);
}

bool Bitmap::ReadPixels(const OH_Drawing_Image_Info *dstInfo, void *dstPixels, size_t dstRowBytes, int32_t srcX,
                        int32_t srcY) const
{
    return OH_Drawing_BitmapReadPixels(bitmap_.get(), dstInfo, dstPixels, dstRowBytes, srcX, srcY);
}

} // namespace LottieTurbo::drawing