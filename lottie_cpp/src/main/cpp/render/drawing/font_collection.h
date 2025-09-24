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

#ifndef LOTTIETURBO_FONT_COLLECTION_H
#define LOTTIETURBO_FONT_COLLECTION_H

#include <memory>
#include <native_drawing/drawing_font_collection.h>

namespace LottieTurbo::drawing {

/**
 * @brief 字体集合类，封装原生OH_Drawing_FontCollection对象
 * 提供对字体集合的管理功能，包括创建、销毁、禁用回退机制等操作
 */
class FontCollection final {
    using UniqueNativeFontCollection = std::unique_ptr<OH_Drawing_FontCollection,
        decltype(&OH_Drawing_DestroyFontCollection)>;

public:
    /**
     * @brief 默认构造函数，创建标准字体集合
     */
    FontCollection();

    /**
     * @brief 析构函数
     */
    ~FontCollection() = default;

    /**
     * @brief 拷贝构造函数
     * @param other 被拷贝的对象
     */
    FontCollection(const FontCollection& other);

    /**
     * @brief 拷贝赋值运算符
     * @param other 被拷贝的对象
     * @return 自身引用
     */
    FontCollection& operator=(const FontCollection& other);

    /**
     * @brief 移动构造函数
     */
    FontCollection(FontCollection&&) = default;

    /**
     * @brief 移动赋值运算符
     */
    FontCollection& operator=(FontCollection&&) = default;

    /**
     * @brief 获取原生字体集合对象指针
     * @return 原生OH_Drawing_FontCollection对象指针
     */
    OH_Drawing_FontCollection* get() const
    {
        return fontCollection_.get();
    }

    /**
     * @brief 创建共享字体集合
     * 创建一个可以在多个Typography对象之间共享使用的字体集合
     * @return 共享字体集合对象
     */
    static FontCollection CreateShared();

    /**
     * @brief 获取全局字体集合实例
     * 
     * 获取系统提供的全局字体集合实例，该实例由系统管理生命周期
     * @return 全局字体集合对象的引用包装
     */
    static FontCollection GetGlobalInstance();

private:
    /**
     * @brief 使用已存在的原生对象构造FontCollection
     * @param fontCollection 原生字体集合对象指针
     * @param shouldDestroy 是否需要在析构时销毁原生对象
     */
    FontCollection(OH_Drawing_FontCollection* fontCollection, bool shouldDestroy);

    UniqueNativeFontCollection fontCollection_;
};

/**
 * @brief 全局字体集合引用包装类
 * 用于包装全局字体集合实例，不负责销毁原生对象
 */
class GlobalFontCollectionRef final {
public:
    /**
     * @brief 构造函数
     * @param fontCollection 全局字体集合原生对象指针
     */
    explicit GlobalFontCollectionRef(OH_Drawing_FontCollection* fontCollection);

    /**
     * @brief 析构函数（不销毁原生对象）
     */
    ~GlobalFontCollectionRef() = default;

    // 禁用拷贝和移动操作
    GlobalFontCollectionRef(const GlobalFontCollectionRef&) = delete;
    GlobalFontCollectionRef& operator=(const GlobalFontCollectionRef&) = delete;
    GlobalFontCollectionRef(GlobalFontCollectionRef&&) = delete;
    GlobalFontCollectionRef& operator=(GlobalFontCollectionRef&&) = delete;

    /**
     * @brief 获取原生字体集合对象指针
     * @return 原生OH_Drawing_FontCollection对象指针
     */
    OH_Drawing_FontCollection* get() const
    {
        return fontCollection_;
    }

private:
    OH_Drawing_FontCollection* fontCollection_;
};

} // namespace LottieTurbo::drawing

#endif // LOTTIETURBO_FONT_COLLECTION_H