# 🔥 opencv-webcam-script（PyPi版）

## 创建人：曾逸夫



<h3 align="center">🚀 安装教程</h3>

#### 📌 第一步：创建虚拟环境

```shell
# 创建conda虚拟环境，以python 3.8为例
conda create -n ows python==3.8 # 虚拟环境名称为ows
conda activate ows # 激活虚拟环境
```



#### 📌 第二步：安装

```shell
pip install opencv-webcam-script==0.5.1 # 以0.5.1为例
```

❗ 注意：`==`后面一定要加**版本号**。如果执行`pip install opencv-webcam-script` 可能安装为测试版，会影响使用。



<h3 align="center">⚡ 使用教程</h3>

#### ✨ 运行程序

```python
from opencv_webcam.opencv_webcam import webcam_opencv

webcam_opencv() # 常规调用
webcam_opencv(is_autoSaveFrame=True) # 自动保存帧
webcam_opencv(is_autoSaveFrame=True, is_compress=True) # 压缩帧
```



#### ✨ 常规调用

```python
webcam_opencv() # 常规调用
```



#### ✨ 选择设备

```python
# 多摄像头切换示例，默认为0
webcam_opencv(device_index=0)
webcam_opencv(device_index=1)
webcam_opencv(device_index=2)
# RTSP
webcam_opencv(device_index="rtsp://username:password@xxx.xxx.xxx")
```



#### ✨ 设置退出键

```python
# 默认按q键退出
webcam_opencv(quit_key="z") # 设置z键退出
webcam_opencv(quit_key="k") # 设置k键退出
```



#### ✨ 自动保存帧

```python
webcam_opencv(is_autoSaveFrame=True)
```



#### ✨ 每隔n帧保存一次帧

```python
# 每隔10帧保存一次帧
webcam_opencv(is_autoSaveFrame=True, frame_nSave=10)
```



#### ✨ 手动保存帧

```python
# 默认按a键捕获一帧
webcam_opencv(is_handSaveFrame=True)
```


#### ✨ 自定义捕获键

```python
# 设置z键为捕获键，默认为a键
webcam_opencv(is_handSaveFrame=True, frame_capKey="z")
```


#### ✨ 重塑帧尺寸（自定义宽高）

```python
# 重塑宽度300 高度200
webcam_opencv(is_autoSaveFrame=True, is_resizeFrame=True, resize_frame="640,480") # 自动版，v0.5
webcam_opencv(is_handSaveFrame=True, is_resizeFrame=True, resize_frame="640,480") # 手动版，v0.5

webcam_opencv(is_autoSaveFrame=True, is_resizeFrame=True, resize_frame=[640, 480]) # 自动版，v0.4之前
webcam_opencv(is_handSaveFrame=True, is_resizeFrame=True, resize_frame=[640, 480]) # 手动版，v0.4之前
```


#### ✨ 重塑帧尺寸（自定义宽高缩放比）

```python
# 宽高缩放比为0.5
webcam_opencv(is_autoSaveFrame=True, is_resizeFrame=True, resizeRatio_frame=0.5) # 自动版
webcam_opencv(is_handSaveFrame=True, is_resizeFrame=True, resizeRatio_frame=0.5) # 手动版
```


#### ✨ 自定义保存路径

```python
# 设置保存路径，默认保存路径为./WebcamFrame
webcam_opencv(is_autoSaveFrame=True, frame_saveDir="custom_dir") # 以自动版为例
```


#### ✨ 自定义帧目录名称

```python
# 设置帧目录名称，默认保存路径为frames
webcam_opencv(is_autoSaveFrame=True, frame_dirName="frames_custom") # 以自动版为例
```


#### ✨ 自定义帧名前缀

```python
# 设置帧图片的前缀名称
webcam_opencv(is_autoSaveFrame=True, frame_namePrefix="webcam") # 以自动版为例
```


#### ✨ 设置帧保存格式

```python
# 设置JPG质量为100，默认为95
webcam_opencv(is_autoSaveFrame=True, frame_saveStyle="jpg", jpg_quality=100) # 以自动版为例

# 设置PNG质量为5，默认为3
webcam_opencv(is_autoSaveFrame=True, frame_saveStyle="png", png_quality=5) # 以自动版为例
```


#### ✨ 设置暂停键

```python
# 设置w键为暂停键，默认为p键
webcam_opencv(pause_key="w") # 按任意键继续
```


#### ✨ 设置保存帧数

```python
# 设置保存100帧
webcam_opencv(is_autoSaveFrame=True, auto_frameNum=100)
```


#### ✨ 日志设置

```python
# 日志文件默认保存在项目根目录，ows.log
webcam_opencv()

# 设置日志文件名称及类型，类型包括.log、.txt、.data
webcam_opencv(logName="ows02.txt")

# 设置日志保存方式，，默认为追加模式
webcam_opencv(logMode="w") # 设置为覆盖模式
```


#### ✨ 视频帧压缩

```python
# 常规压缩，默认ows.zip（以自动版为例）
webcam_opencv(is_autoSaveFrame=True, is_compress=True)

# 自定义压缩文件名称
webcam_opencv(is_autoSaveFrame=True, is_compress=True, compressName="ows02")

# 自动命名压缩文件
webcam_opencv(is_autoSaveFrame=True, is_compress=True, is_autoCompressName=True)

# 自定义压缩模式，默认为写覆盖
webcam_opencv(is_autoSaveFrame=True, is_compress=True, compressStyle="tar", compressMode="w:gz") # tar压缩
webcam_opencv(is_autoSaveFrame=True, is_compress=True, compressMode="a") # 追加模式(a模式仅限zip) 注：该指令仅限v0.4
```



#### 💡 使用参数参考`opencv_webcam.py`源码

```python
def webcam_opencv(device_index="0",                 # 设备号
                  quit_key="q",                     # 退出键
                  pause_key="p",                    # 暂停键
                  is_autoSaveFrame=False,           # 自动保存帧
                  frame_saveDir="./WebcamFrame",    # 帧保存路径
                  frame_dirName="frames",           # 帧目录
                  frame_nSave=1,                    # 每隔n帧保存一次
                  auto_frameNum=0,                  # 自动保存最大帧数
                  is_handSaveFrame=False,           # 手动保存帧
                  frame_capKey="a",                 # 设置帧捕获键
                  is_resizeFrame=False,             # 重塑帧
                  resize_frame="640,480",           # 自定义帧尺寸
                  resizeRatio_frame=1.0,            # 自定义帧缩放比
                  frame_namePrefix="frame",         # 自定义帧前缀
                  frame_saveStyle="jpg",            # 帧保存类型
                  jpg_quality=95,                   # jpg质量系数
                  png_quality=3,                    # png质量系数
                  logName="ows.log",                # 日志名称
                  logMode="a",                      # 日志模式
                  is_compress=False,                # 压缩帧
                  compressStyle="zip",              # 压缩类型
                  is_autoCompressName=False,        # 自动命名压缩文件
                  compressName="ows",               # 自定义压缩文件名称
                  compressMode="w"):                # 压缩模式

```

