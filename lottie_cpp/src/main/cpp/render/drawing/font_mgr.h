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

#ifndef LOTTIETURBO_FONT_MGR_H
#define LOTTIETURBO_FONT_MGR_H

#include <memory>
#include <native_drawing/drawing_font_mgr.h>
#include <native_drawing/drawing_typeface.h>
#include <string>
#include <vector>

namespace LottieTurbo::drawing {

/**
 * @brief 字体样式集合类，封装原生OH_Drawing_FontStyleSet对象
 *
 * 提供对字体样式集合的操作，包括创建特定样式的字体、获取样式信息等功能
 */
class FontStyleSet final {
    // 使用智能指针管理native font style set资源
    using UniqueNativeFontStyleSet =
        std::unique_ptr<OH_Drawing_FontStyleSet, decltype(&OH_Drawing_FontMgrDestroyFontStyleSet)>;

public:
    /**
     * @brief 默认构造函数
     */
    FontStyleSet();

    /**
     * @brief 使用已存在的OH_Drawing_FontStyleSet构造
     *
     * @param fontStyleSet 原生字体样式集合对象
     */
    explicit FontStyleSet(OH_Drawing_FontStyleSet *fontStyleSet);

    /**
     * @brief 析构函数
     */
    ~FontStyleSet() noexcept = default;

    /**
     * @brief 根据索引创建字体类型
     *
     * @param index 字体类型在集合中的索引
     * @return 创建的字体类型指针，失败返回nullptr
     */
    OH_Drawing_Typeface *CreateTypeface(int index);

    /**
     * @brief 获取指定索引处的字体样式
     *
     * @param index 样式索引
     * @param styleName [out] 接收样式名称的字符串
     * @return 字体样式结构体
     */
    OH_Drawing_FontStyleStruct GetStyle(int32_t index, std::string &styleName);

    /**
     * @brief 匹配最接近的字体类型
     *
     * @param fontStyle 要匹配的字体样式
     * @return 匹配到的字体类型指针
     */
    OH_Drawing_Typeface *MatchStyle(OH_Drawing_FontStyleStruct fontStyle);

    /**
     * @brief 获取字体类型数量
     *
     * @return 字体样式集合中的字体类型数量
     */
    int Count() const;

    /**
     * @brief 获取原生字体样式集合对象
     *
     * @return 原生字体样式集合对象指针
     */
    OH_Drawing_FontStyleSet *get() const
    {
        return fontStyleSet_.get();
    }

private:
    UniqueNativeFontStyleSet fontStyleSet_;
};

/**
 * @brief 字体管理器类，封装原生OH_Drawing_FontMgr对象
 *
 * 提供对系统字体的管理功能，包括枚举字体家族、创建特定字体等
 */
class FontMgr final {
    // 使用智能指针管理native font manager资源
    using UniqueNativeFontMgr = std::unique_ptr<OH_Drawing_FontMgr, decltype(&OH_Drawing_FontMgrDestroy)>;

public:
    /**
     * @brief 构造函数，创建字体管理器
     */
    FontMgr();

    /**
     * @brief 拷贝构造函数
     *
     * @param other 被拷贝对象
     */
    FontMgr(FontMgr const &other);

    /**
     * @brief 拷贝赋值运算符
     *
     * @param other 被拷贝对象
     * @return 自身引用
     */
    FontMgr &operator=(FontMgr const &other);

    /**
     * @brief 移动构造函数
     */
    FontMgr(FontMgr &&) = default;

    /**
     * @brief 移动赋值运算符
     */
    FontMgr &operator=(FontMgr &&) = default;

    /**
     * @brief 析构函数
     */
    ~FontMgr() noexcept = default;

    /**
     * @brief 获取字体家族数量
     *
     * @return 字体家族数量
     */
    int GetFamilyCount() const;

    /**
     * @brief 获取指定索引处的字体家族名称
     *
     * @param index 字体家族索引
     * @return 字体家族名称
     */
    std::string GetFamilyName(int index) const;

    /**
     * @brief 根据索引创建字体样式集合
     *
     * @param index 字体家族索引
     * @return 创建的字体样式集合
     */
    std::unique_ptr<FontStyleSet> CreateFontStyleSet(int index) const;

    /**
     * @brief 根据字体家族名称匹配字体样式集合
     *
     * @param familyName 字体家族名称
     * @return 匹配的字体样式集合
     */
    std::unique_ptr<FontStyleSet> MatchFamily(const std::string &familyName) const;

    /**
     * @brief 根据字体家族名称和样式匹配字体类型
     *
     * @param familyName 字体家族名称
     * @param fontStyle 字体样式
     * @return 匹配的字体类型指针
     */
    OH_Drawing_Typeface *MatchFamilyStyle(const std::string &familyName, OH_Drawing_FontStyleStruct fontStyle);

    /**
     * @brief 根据字体家族名称、样式和字符匹配字体类型
     *
     * @param familyName 字体家族名称
     * @param fontStyle 字体样式
     * @param bcp47 语言标识符数组，指示字符的语言
     * @param character UTF-8字符值
     * @return 匹配的字体类型指针
     */
    OH_Drawing_Typeface *MatchFamilyStyleCharacter(const std::string &familyName, OH_Drawing_FontStyleStruct fontStyle,
                                                   const std::vector<std::string> &bcp47, int32_t character);

    /**
     * @brief 获取原生字体管理器对象
     *
     * @return 原生字体管理器对象指针
     */
    OH_Drawing_FontMgr *get() const
    {
        return fontMgr_.get();
    }

private:
    UniqueNativeFontMgr fontMgr_;
};

} // namespace LottieTurbo::drawing

#endif