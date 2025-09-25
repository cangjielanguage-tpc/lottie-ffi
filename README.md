<div align="center">
<h1>lottie-turbo</h1>
</div>

<p align="center">
<img alt="" src="https://img.shields.io/badge/release-v1.0.0-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/build-pass-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/cjc-v1.0.0-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/cjcov-NA-brightgreen" style="display: inline-block;" />
<img alt="" src="https://img.shields.io/badge/project-open-brightgreen" style="display: inline-block;" />
</p>

## 介绍

lottie-turbo是一个动画库，它可以解析Adobe After Effects软件通过Bodymovin插件导出的json格式的动画，并在移动设备上进行本地渲染。

## 特性

- 🚀 支持加载本地文件方式加载动画
- 🚀 支持加载网络资源以URI路径方式加载动画
- 🚀 支持动画播放、暂停、销毁等功能

### 源码目录

```shell
/lottie-turbo                   # 项目根目录
├── doc                      #文档目录
│   ├── assets               #文档资源目录
│   └── feature_api.md       #API接口文档
├── entry                    # 示例代码文件夹
├── lottie                   # lottie库文件夹
│    └─ src/main/cangjie/src # 核心代码，包含json解析，动画绘制，操作动画                  
├── README.md                # 安装使用方法      
```
### 接口说明

主要类和函数接口说明详见 [API](doc/feature_api.md)

## 使用说明

### 编译构建
1. 通过module引入
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
2.三方库方式引入
    1. 目标工程把 lottie-turbo 依赖库作为 git submodule 引入
    ```
    > cd $工程根目录
    > mkdir third-party
    > cd third-party
    > git submodule add "https://gitcode.com/Cangjie-TPC/lottie-turbo.git"
    ```
    2. 修改自身应用 entry/src/main/cangjie 下的 cjpm.toml 文件，添加依赖
    ```
      [dependencies]
        lottie = {path = "../../../../third-party/lottie-turbo/lottie/src/main/cangjie", version = "1.0.0"}
    ```
        或
    ```
    [dependencies]
      [dependencies.lottie-turbo]
        path = "../../../../third-party/lottie-turbo/lottie/src/main/cangjie"
        version = "1.0.0"
    ```

    3. 在项目中引用
    ```cangjie
    import lottie.*
    ````
### 功能示例

```cangjie
import ohos.base.*
import ohos.component.*
import ohos.state_manage.*
import ohos.state_macro_manage.*
import lottie.*

@Entry
@Component
class Index {
    @State
    var controller: LottieController = LottieController()
    public func build() {
        Column {
              LottieView(
                lottieId: "lottie1", //动画id，需要保证唯一性
                loop: true, //是否循环播放，非必须，默认为true
                autoplay: true, //是否自动播放，非必须，默认为true
                autoSkip: true, //不可见时是否自动跳过渲染，非必须，默认为true
                path: "/data/storage/el1/bundle/entry/resources/resfile/grunt.json", //通过resfile文件播放
                // path: "https://cdn.lottielab.com/l/7Zgk9iuQxmZ3tD.json", //通过uri播放
                controller: this.controller //lottie动画控制器
              ).height(50.percent).width(160)
                .backgroundColor(Color.GRAY)
                .onClick {
                    =>
                    this.controller.togglePause(); //控制动画播放暂停
                }
        }.height(100.percent).width(100.percent)
    }
}
```

## 约束与限制

在下述版本验证通过：

    IDE: DevEco Studio 5.1.1 Release(Build Version: 5.1.1.823)

## 开源协议

本项目基于 [Apache License 2.0](./LICENSE)，请自由的享受和参与开源。

## 参与贡献

欢迎给我们提交 PR，欢迎给我们提交 Issue，欢迎参与任何形式的贡献。