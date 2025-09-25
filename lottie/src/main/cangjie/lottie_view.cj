/*
 * Copyright (c) Huawei Technologies Co., Ltd. 2025-2025. All rights reserved.
 */

package lottie

import std.time.*

import ohos.display.*

internal import kit.UIKit.*
import ohos.state_macro_manage.*


@Component
public class LottieView {

    var config: AnimationConfig
    var controller: LottieController

    func build() {
        Column() {
            XComponent(id: this.controller.lottieId, `type`: XComponentType.TEXTURE, controller: controller)
                .width(100.percent)
                .height(100.percent)
        }.width(100.percent).height(100.percent)
    }

    protected func aboutToAppear(): Unit {
        if (this.config.runOnce) {
            return
        }
        //TODO:lipei---------------[暂未处理]------------------
        //this.config.runOnce = true
        var lottieId: String = this.config.lottieId
        if (lottieId == "") {
            /*
             * arkts 是 getUniqueId() 获取自定义组件的 uniqueId。rlottie 参考 libpag 是 c 侧 UniqueID::Next() 获取 UniqueID 拼接字符串。
             */
            lottieId = getUniqueId()
        }

        this.controller.lottieId = lottieId
        this.config.lottieId = lottieId

        let displayInfo: Display = getDefaultDisplaySync();
        let animationConfig: AnimationConfig = AnimationConfig(
            lottieId: lottieId,
            path: this.config.path,
            radius: this.config.radius,
            animationData: this.config.animationData,
            loop: this.config.loop,
            autoplay: this.config.autoplay,
            initialSegment: this.config.initialSegment,
            contentMode: this.config.contentMode,
            frameRate: this.config.frameRate,
            useCache: this.config.useCache,
            autoSkip: this.config.autoSkip,
            controller: this.controller.getPtr(),
            listener: this.config.listener,
            density: Float64(displayInfo.densityDPI),
            setImageAssetDelegate: this.config.setImageAssetDelegate,
            animator: this.controller.getAnimatorPtr()
        )
        this.config = animationConfig

        createNativeRoot(
            lottieId,
            animationConfig
        )

        if (this.config.autoSkip && this.config.visible) {
            //TODO:lipei-----------------------------------[未实现]-----------------------------------------------
            lottieSetVisible(this.config.lottieId, true);
        } else if (this.config.autoSkip && !this.config.visible) {
            //TODO:lipei-----------------------------------[未实现]-----------------------------------------------
            lottieSetVisible(this.config.lottieId, false);
        }
    }

    protected func aboutToDisappear(): Unit {
        this.controller.destroy()
    }

}
