package ohos_app_cangjie_entry

import ohos.state_macro_manage.*

internal import ohos.base.*
internal import ohos.component.*
internal import ohos.state_manage.*
internal import ohos.resource_manager.*
internal import ohos.hilog.Hilog
internal import ohos.ability.*

internal import lottie.*

@Entry
@Component
class EntryView {

    var controller: LottieController = LottieController()
    @State var animationData: String = ""
    var fileName: String = "grunt.json"

    func build() {
        Column(20) {

            Column {
                LottieView(config: AnimationConfig(
                    path: "/data/storage/el1/bundle/entry/resources/resfile/navidad.json",
                    loop: 0,
                    autoplay: false,
                    contentMode: "Contain"
                ), controller: controller)
            }.width(50.percent).height(160).backgroundColor(Color.GRAY)

            List() {
                ListItem() {
                    Column(5) {
                        Button('核心功能').width(100.percent).height(35).shape(ShapeType.Normal)
                        //*********************************************************************************
                        //核心功能***************************************************************************
                        Row(5) {
                            Button('reload')
                                .width(35.percent)
                                .height(35)
                                .shape(ShapeType.Normal)
                                .onClick {
                                    =>
                                    this.controller.reload(AnimationConfig(
                                      loop: 2,
                                      autoplay: true,
                                      path: "/data/storage/el1/bundle/entry/resources/resfile/animation.json"
                                    ))
                                }
                        }

                        Row(5) {
                            Button('play')
                                .width(33.percent)
                                .height(35)
                                .shape(ShapeType.Normal)
                                .onClick {
                                    =>
                                    this.controller.play()
                                }
                            Button('pause')
                                .width(33.percent)
                                .height(35)
                                .shape(ShapeType.Normal)
                                .onClick {
                                    =>
                                    this.controller.pause()
                                }
                            Button('destroy')
                                .width(33.percent)
                                .height(35)
                                .shape(ShapeType.Normal)
                                .onClick {
                                    =>
                                    Hilog.error(0, "lipei", "[destroy定位]---------------------[执行 controller.destroy() 函数]------------------")
                                    Hilog.error(0, "lipei", "[destroy定位]---------------------[执行 controller.destroy() 函数]------------------")
                                    Hilog.error(0, "lipei", "[destroy定位]---------------------[执行 controller.destroy() 函数]------------------")
                                    this.controller.destroy()
                                }
                        }

                        Row(5) {
                            Button('stop')
                                .width(50.percent)
                                .height(35)
                                .shape(ShapeType.Normal)
                                .onClick {
                                    =>
                                    this.controller.stop()
                                }
                            Button('togglePause')
                                .width(50.percent)
                                .height(35)
                                .shape(ShapeType.Normal)
                                .onClick {
                                    =>
                                    this.controller.togglePause()
                                }
                        }

                    }.width(100.percent)
                }
            }.width(100.percent).height(90.percent).scrollBar(BarState.Off)

            //Button('API列表')
            //    .width(90.percent)
            //    .height(50)
            //    .shape(ShapeType.Normal)
            //    .onClick {
            //        =>
            //        // API列表页面api_list.cj
            //        Router.push(url: "ApiList")
            //    }
        }.width(100.percent).height(100.percent).padding(top: 20, left: 10, right: 10)
    }

    //protected func aboutToAppear(): Unit {
    //    let buffer: Array<UInt8> = loadPath(fileName, globalAbilityContext)
    //    this.animationData = String.fromUtf8(buffer)
    //    Hilog.error(0, "lipei", "begin---------------this.animationData.size = ${this.animationData.size}")
    //}

}
