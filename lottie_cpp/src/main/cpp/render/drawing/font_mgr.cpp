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

#include "font_mgr.h"

namespace LottieTurbo::drawing {

// FontStyleSet 实现部分
FontStyleSet::FontStyleSet() : fontStyleSet_(nullptr, &OH_Drawing_FontMgrDestroyFontStyleSet)
{
}

FontStyleSet::FontStyleSet(OH_Drawing_FontStyleSet *fontStyleSet)
    : fontStyleSet_(fontStyleSet, &OH_Drawing_FontMgrDestroyFontStyleSet)
{
}

OH_Drawing_Typeface *FontStyleSet::CreateTypeface(int index)
{
    return OH_Drawing_FontStyleSetCreateTypeface(fontStyleSet_.get(), index);
}

OH_Drawing_FontStyleStruct FontStyleSet::GetStyle(int32_t index, std::string &styleName)
{
    char *name = nullptr;
    auto style = OH_Drawing_FontStyleSetGetStyle(fontStyleSet_.get(), index, &name);
    if (name != nullptr) {
        styleName = name;
        OH_Drawing_FontStyleSetFreeStyleName(&name);
    }
    return style;
}

OH_Drawing_Typeface *FontStyleSet::MatchStyle(OH_Drawing_FontStyleStruct fontStyle)
{
    return OH_Drawing_FontStyleSetMatchStyle(fontStyleSet_.get(), fontStyle);
}

int FontStyleSet::Count() const
{
    return OH_Drawing_FontStyleSetCount(fontStyleSet_.get());
}

// FontMgr 实现部分
FontMgr::FontMgr() : fontMgr_(UniqueNativeFontMgr(OH_Drawing_FontMgrCreate(), &OH_Drawing_FontMgrDestroy))
{
}

FontMgr::FontMgr(FontMgr const &other)
    : fontMgr_(UniqueNativeFontMgr(OH_Drawing_FontMgrCreate(), &OH_Drawing_FontMgrDestroy))
{
    // 字体管理器不需要复制具体内容，因为它是系统级资源管理器
}

FontMgr &FontMgr::operator=(FontMgr const &other)
{
    if (this != &other) {
        fontMgr_ = UniqueNativeFontMgr(OH_Drawing_FontMgrCreate(), &OH_Drawing_FontMgrDestroy);
    }
    return *this;
}

int FontMgr::GetFamilyCount() const
{
    return OH_Drawing_FontMgrGetFamilyCount(fontMgr_.get());
}

std::string FontMgr::GetFamilyName(int index) const
{
    char *name = OH_Drawing_FontMgrGetFamilyName(fontMgr_.get(), index);
    std::string familyName;
    if (name != nullptr) {
        familyName = name;
        OH_Drawing_FontMgrDestroyFamilyName(name);
    }
    return familyName;
}

std::unique_ptr<FontStyleSet> FontMgr::CreateFontStyleSet(int index) const
{
    auto styleSet = OH_Drawing_FontMgrCreateFontStyleSet(fontMgr_.get(), index);
    return std::make_unique<FontStyleSet>(styleSet);
}

std::unique_ptr<FontStyleSet> FontMgr::MatchFamily(const std::string &familyName) const
{
    auto styleSet = OH_Drawing_FontMgrMatchFamily(fontMgr_.get(), familyName.c_str());
    return std::make_unique<FontStyleSet>(styleSet);
}

OH_Drawing_Typeface *FontMgr::MatchFamilyStyle(const std::string &familyName, OH_Drawing_FontStyleStruct fontStyle)
{
    return OH_Drawing_FontMgrMatchFamilyStyle(fontMgr_.get(), familyName.c_str(), fontStyle);
}

OH_Drawing_Typeface *FontMgr::MatchFamilyStyleCharacter(const std::string &familyName,
                                                        OH_Drawing_FontStyleStruct fontStyle,
                                                        const std::vector<std::string> &bcp47, int32_t character)
{
    std::vector<const char *> bcp47Ptrs;
    bcp47Ptrs.reserve(bcp47.size());
    for (const auto &lang : bcp47) {
        bcp47Ptrs.push_back(lang.c_str());
    }

    return OH_Drawing_FontMgrMatchFamilyStyleCharacter(fontMgr_.get(), familyName.c_str(), fontStyle, bcp47Ptrs.data(),
                                                       static_cast<int>(bcp47Ptrs.size()), character);
}

} // namespace LottieTurbo::drawing