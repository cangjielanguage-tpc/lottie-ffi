<div align="center">
<h1>lottie-ffi</h1>
</div>

<p align="center">
<img alt="" src="https://img.shields.io/badge/release-v1.0.2-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/build-pass-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/cjc-v1.1.3-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/cjcov-81.3%25-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/project-open-brightgreen" style="display: inline-block;" />
</p>

## 介绍

lottie-ffi是一个动画库，它可以解析Adobe After Effects软件通过Bodymovin插件导出的json格式的动画，并在移动设备上进行本地渲染。

## 特性

- 🚀 支持通过沙箱路径方式加载动画文件
- 🚀 支持加载网络资源以URI路径方式加载动画
- 🚀 支持动画播放、暂停、销毁等功能

### 源码目录

```shell
/lottie-ffi                  # 项目根目录
├── doc                      #文档目录
│   ├── assets               #文档资源目录
│   └── feature_api.md       # API 接口文档
├── entry                    # 示例代码文件夹
├── lottie                   # lottie 库文件夹
│    └─ src/main/cangjie/src # 核心代码，包含 json 解析，动画绘制，操作动画      
├── lottie_cpp               # lottie 库 c 代码文件夹
│    └─ src/main/cpp/cangjie # 核心 c 代码，包含 json 解析，动画绘制，操作动画                
├── README.md                # 安装使用方法      
```
### 接口说明

主要类和函数接口说明详见 [API](doc/feature_api.md)

## 使用说明

### 编译构建
#### 1. 通过module引入
    1. 克隆下载项目
    2. 将lottie模块拷贝到应用项目下
    3. 修改自身应用 entry 下的 oh-package.json5 文件，在 dependencies 字段添加 "lottie": "file:../lottie"
       ```json
       {
          "name": "entry",
          "version": "1.0.0",
          "description": "Please describe the basic information.",
          "main": "",
          "author": "",
          "license": "",
          "dependencies": {
            "lottie": "file:../lottie"
          }
        }
       ```
    4. 在项目中使用 import lottie.* 引用lottie项目
       ```cangjie
       import lottie.*
       ```

#### 2. 三方库方式引入
    1. 目标工程把 lottie-ffi 依赖库作为 git submodule 引入
    ```
    > cd $工程根目录
    > mkdir third-party
    > cd third-party
    > git submodule add "https://gitcode.com/Cangjie-TPC/lottie-ffi.git"
    ```
    2. 修改自身应用 entry/src/main/cangjie 下的 cjpm.toml 文件，添加依赖
    ```
      [dependencies]
        lottie = {path = "../../../../third-party/lottie-ffi/lottie/src/main/cangjie", version = "1.0.0"}
    ```
        或
    ```
    [dependencies]
      [dependencies.lottie-ffi]
        path = "../../../../third-party/lottie-ffi/lottie/src/main/cangjie"
        version = "1.0.0"
    ```

    3. 在项目中引用
    ```cangjie
    import lottie.*
    ````

> 路径资源在工程entry/src/main/resources/下创建新文件夹，例如 resfile。


### 功能示例

```cangjie
import ohos.base.*
import ohos.component.*
import ohos.state_manage.*
import ohos.state_macro_manage.*
import lottie.*

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
                    lottieId: "lottie1",        //动画id，需要保证唯一性
                    path: "/data/storage/el1/bundle/entry/resources/resfile/navidad.json",
                    loop: 0,                    // 是否循环播放,默认 -1 循环。
                    autoplay: false,            // 是否自动播放，默认 true
                    contentMode: "Contain"      // 填充的模式
                ), controller: controller)
            }.width(50.percent).height(160).backgroundColor(Color.GRAY)

            List() {
                ListItem() {
                    Column(5) {
                        Row(5) {
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
        }.width(100.percent).height(100.percent).padding(top: 20, left: 10, right: 10)
    }

}
```

### 注意事项

- 1.当前默认在页面 aboutToDisappear时自动触发 destroy 进行销毁。
- 2.需要注意 lottieId 的唯一性，不配置时将自动随机生成。
- 3.path 支持沙箱路径、uri下载链接。

## 约束与限制

在下述版本验证通过：

    DevEco Studio 6.1.1 Release
    Cangjie Support Plugin 6.1.1.280
    Cangjie Compiler: 1.1.3 (cjnative)

## 开源协议

本项目基于 [Apache License 2.0](./LICENSE)，请自由的享受和参与开源。

## 参与贡献

欢迎给我们提交 PR，欢迎给我们提交 Issue，欢迎参与任何形式的贡献。