## 开始
- ### SeetaFace 1.0 引擎
    `SeetaFace` 是中科院计算机所山世光老师所带领的团队开发出来的人脸识别库。
    > SeetaFace 2.0 已开源。
- ### 模块三个独立的模块
    - `FaceDetection` 人脸检测
    - `FaceAlignment` 人脸对齐
    - `FaceIdentification` 人脸识别

## 开源
[![License](https://img.shields.io/badge/license-BSD-blue.svg)](LICENSE)

## 编译
- ### OpenCV
    - 下载并安装 `opencv-2.4.12.exe` 或 [百度网盘](https://pan.baidu.com/s/1l0X0-q_22We1K9DqIXGXxg)。
- ### NSeetaface
    - 修改 `NSeetaface` : `项目` > `属性` > `配置属性` > `VC++目录` 
        - `包含目录` 添加 opencv 的安装目录 
            - `opencv\build\include`
            - `opencv\build\include\opencv`
            - `opencv\build\include\opencv2`
        - `库目录` 添加 opencv 的安装目录 `opencv\build\x86\vc12\lib`
    - 切换解决方案配置为 `Release`、解决方案平台为 `x86`。
    - 设置 `ViewFace` 为启动项。
    - 生成解决方案。
- ### 生成
    - 输出目录 `SeetaFaceEngine.Net\NSeetaface\Release`
    - 输出目录 `SeetaFaceEngine.Net\NSeetaface\Release\netstandard2.0`

## 使用
> 自行编译 或 [下载 Release.zip](https://github.com/View12138/SeetaFaceEngine.Net/releases) 或 [百度网盘](https://pan.baidu.com/s/1FTn3ptVimCuy9JHrlFl5fQ)

- 解压 `Release.zip` ，将 `model/` 、`FaceAlignment.dll`、`FaceDetection.dll`、`Identification.dll`、`NSeetaface.dll` 拷贝至项目的生成的目录。
- 在项目中引用 `ViewFace.dll` 