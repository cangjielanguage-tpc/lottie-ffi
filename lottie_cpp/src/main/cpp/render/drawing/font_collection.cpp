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

#include "font_collection.h"

namespace LottieTurbo::drawing {

// FontCollection 实现
FontCollection::FontCollection()
    : fontCollection_(UniqueNativeFontCollection(OH_Drawing_CreateFontCollection(), &OH_Drawing_DestroyFontCollection))
{
}

FontCollection::FontCollection(OH_Drawing_FontCollection *fontCollection, bool shouldDestroy)
    : fontCollection_(shouldDestroy ? UniqueNativeFontCollection(fontCollection, &OH_Drawing_DestroyFontCollection)
                                    : UniqueNativeFontCollection(fontCollection, [](OH_Drawing_FontCollection *) {}))
{
}

FontCollection::FontCollection(const FontCollection& other)
    : fontCollection_(UniqueNativeFontCollection(OH_Drawing_CreateFontCollection(), &OH_Drawing_DestroyFontCollection))
{
}

FontCollection& FontCollection::operator=(const FontCollection& other)
{
    if (this != &other) {
        // 重新创建字体集合
        fontCollection_ = UniqueNativeFontCollection(OH_Drawing_CreateFontCollection(),
                                                     &OH_Drawing_DestroyFontCollection);
    }
    return *this;
}

FontCollection FontCollection::CreateShared()
{
    auto sharedCollection = OH_Drawing_CreateSharedFontCollection();
    return FontCollection(sharedCollection, true);
}

// GlobalFontCollectionRef 实现
GlobalFontCollectionRef::GlobalFontCollectionRef(OH_Drawing_FontCollection* fontCollection)
    : fontCollection_(fontCollection)
{
}
} // namespace LottieTurbo::drawing