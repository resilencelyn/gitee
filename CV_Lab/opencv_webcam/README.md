中文简体|[English](./README.en.md)
<p align="center">
<a href="https://gitee.com/CV_Lab/opencv_webcam">
<img src="https://pic.imgdb.cn/item/61f116412ab3f51d918b7bb1.png" alt="Simple Icons" >
</a>
<p align="center">
    基于OpenCV的Webcam程序，适用于计算机视觉数据采集，实时视频帧收集等。
</p>
<p align="center">
    具有安装便捷、操作简单、跨平台等特点。
</p>
</p>
<p align="center">
<a href="https://gitee.com/CV_Lab/opencv_webcam/blob/master/CodeCheck.md"><img src="https://img.shields.io/badge/CodeCheck-passing-success" alt="code check" /></a>
<a href="https://gitee.com/CV_Lab/opencv_webcam/releases/v0.5"><img src="https://img.shields.io/badge/Releases-v0.5-green" alt="Releases Version" /></a>
<a href="https://pypi.org/project/opencv-webcam-script/0.5.1/"><img src="https://img.shields.io/badge/PyPI-v0.5.1-brightgreen?logo=pypi" alt="PyPI" /></a>
<a href="https://hub.docker.com/r/zengdockerdocker/opencv-webcam-script"><img src="https://img.shields.io/badge/docker-v0.5-blue?logo=docker" alt="Docker" /></a>
<a href="./tutorial/ows_podman.md"><img src="https://img.shields.io/badge/Podman-v0.5-blueviolet?logo=podman" alt="Podman" /></a>
<a href="https://gitee.com/CV_Lab/opencv_webcam/blob/master/tutorial/ows_jupyter.ipynb"><img src="https://img.shields.io/badge/Jupyter-v0.5-orange?logo=jupyter" alt="Jupyter" /></a>
<a href="https://gitee.com/CV_Lab/opencv_webcam/blob/master/LICENSE"><img src="https://img.shields.io/badge/License-GPL--3.0-blue" alt="License" /></a>
</p>
<p align="center">
<a href="#"><img src="https://img.shields.io/badge/Python-3.8%2B-blue?logo=python" alt="Python Version" /></a>
<a href='https://gitee.com/CV_Lab/opencv_webcam/stargazers'><img src='https://gitee.com/CV_Lab/opencv_webcam/badge/star.svg?theme=dark' alt='star'></img></a>
<a href='https://gitee.com/CV_Lab/opencv_webcam/members'><img src='https://gitee.com/CV_Lab/opencv_webcam/badge/fork.svg?theme=dark' alt='fork'></img></a>
<a href="https://hub.docker.com/r/zengdockerdocker/opencv-webcam-script"><img src="https://img.shields.io/docker/pulls/zengdockerdocker/opencv-webcam-script?logo=docker" alt="Docker Pulls"></a>
<a href="https://pypi.org/project/opencv-webcam-script"><img src="https://static.pepy.tech/personalized-badge/opencv-webcam-script?period=total&units=international_system&left_color=grey&right_color=brightgreen&left_text=PyPI%20downloads" alt="PyPI Downloads"></a>
<a href="https://snyk.io/advisor/python/opencv-webcam-script"><img src="https://snyk.io/advisor/python/opencv-webcam-script/badge.svg" alt="PyPI Package Health"></a>
</p>




<h2 align="center">🚀更新走势</h2>

- `2022-02-06` **⚡ [opencv-webcam-script v0.5 发行版](https://gitee.com/CV_Lab/opencv_webcam/releases/v0.5)正式上线**
- `2022-02-06` **⚡ [opencv-webcam-script v0.5 docker版](https://hub.docker.com/r/zengdockerdocker/opencv-webcam-script)正式上线**
- `2022-02-06` **⚡ [opencv-webcam-script v0.5.1 PyPI版](https://pypi.org/project/opencv-webcam-script/0.5.1/)正式上线**
- `2022-02-06` **⚡ [opencv-webcam-script v0.5 Jupyter版](https://gitee.com/CV_Lab/opencv_webcam/blob/master/tutorial/ows_jupyter.ipynb)正式上线**
- ⚡ **[opencv-webcam-script 历史版本](./v_change)**



<h2 align="center">💎项目用途</h2>

### 🔥 快速制作目标检测数据集
####  📌 项目1：[基于YOLOv5的手势识别demo](GestureData_yolov5.md)

<p align="center" >
<a href="https://gitee.com/CV_Lab/opencv_webcam/blob/master/GestureData_yolov5.md">
<img src="https://pic.imgdb.cn/item/61f9be4e2ab3f51d9184300d.gif"></a>
</p>


<h2 align="center">💡项目结构</h2>

```
.
├── opencv_webcam							# 项目名称
│   ├── utils								# 工具包
│   │   ├── __init__.py						# 工具包初始化
│   │   ├── args_yaml.py					# 指令管理
│   │   ├── compress.py						# 压缩管理
│   │   ├── frame_opt.py					# 帧保存管理
│   │   ├── hotkey.py						# 热键管理
│   │   ├── fonts_opt.py					# 字体管理
│   │   ├── plot.py							# 作图管理
│   │   ├── log.py							# 日志管理
│   │   ├── ows_path.py						# 保存路径管理
│   │   └── time_format.py					# 时间格式化
│   ├── tutorial							# ows教程
│   │   ├── ows_pypi.md						# ows PyPI版教程
│   │   ├── ows_docker.md					# ows docker版教程
│   │   ├── ows_podman.md					# ows Podman版教程
│   │   └── ows_jupyter.ipynb				# ows Jupyter版教程
│   ├── v_change							# 版本历史
│   │   ├── v01_change.md					# v0.1版本变更
│   │   ├── v02_change.md					# v0.2版本变更
│   │   ├── v03_change.md					# v0.3版本变更
│   │   ├── v04_change.md					# v0.4版本变更
│   │   └── README.md						# 版本历史说明
│   ├── __init__.py							# 项目初始化
│   ├── opencv_webcam.py					# 脚本主运行文件
│   ├── LICENSE								# 项目许可
│   ├── CodeCheck.md						# 代码检查
│   ├── setup.sh							# 环境安装脚本
│   ├── fonts.sh							# 字体下载脚本
│   ├── Dockerfile							# docker构建文件
│   ├── .gitignore							# git忽略文件
│   ├── .dockerignore						# docker忽略文件
│   ├── GestureData_yolov5.md				# Gesture&YOLOv5 demo
│   ├── README.md							# 项目说明
│   ├── README.en.md						# 项目说明（英文版）
│   └── requirements.txt					# 脚本依赖包
```



<h2 align="center">🔥安装教程</h2>

❤️ OWS提供了**5**种安装方法。点击下面对应的logo，了解详细的安装与使用教程。

<div align="center">
    <a href="https://gitee.com/CV_Lab/opencv_webcam">
        <img src="https://pic.imgdb.cn/item/620f35522ab3f51d91944fda.png" width="15%"/>
    </a>
    <a href="./tutorial/ows_pypi.md">
        <img src="https://pic.imgdb.cn/item/620ef2be2ab3f51d913907a7.png" width="18%"/>
    </a>
    <a href="./tutorial/ows_docker.md">
        <img src="https://pic.imgdb.cn/item/620eeea32ab3f51d913428fe.png" width="15%"/>
    </a>
    <a href="./tutorial/ows_podman.md">
        <img src="https://pic.imgdb.cn/item/620eeea42ab3f51d91342afc.png" width="15%"/>
    </a>
    <a href="./tutorial/ows_jupyter.ipynb">
        <img src="https://pic.imgdb.cn/item/620ef2c02ab3f51d913909af.png" width="12%"/>
    </a>
</div>



### ✅ 方法一：Linux Shell安装（开发版）

📌 第一步：克隆项目

```shell
git clone https://gitee.com/CV_Lab/opencv_webcam.git # 克隆项目
```

📌 第二步：创建虚拟环境

```shell
# 创建conda虚拟环境，以python 3.8为例
conda create -n ows python==3.8 # 虚拟环境名称为ows
conda activate ows # 激活虚拟环境
```

📌 第三步：安装脚本

```shell
pip install -r ./requirements.txt -U # 安装OpenCV Webcam脚本
```

📌 第二步和第三步也可以通过[setup.sh](./setup.sh) 执行

```shell
bash ./setup.sh
```

❗ 注意：**OWS v0.6** 以上的版本需要在[**Python>=3.8.0**](https://www.python.org/) 的环境下运行



### ✅ 方法二：pip 快速安装（PyPI版）

#### 🔥 [opencv-webcam-script PyPI版 详细教程](./tutorial/ows_pypi.md)

#### 🎨 简易教程

📌 第一步：创建ows虚拟环境，参见方法一

📌 第二步：执行pip指令

```shell 
pip install opencv-webcam-script==0.5.1
```

📌 第三步：编写python程序

```python
from opencv_webcam.opencv_webcam import webcam_opencv

# ------------例举几个功能------------
webcam_opencv() # 常规调用
webcam_opencv(is_autoSaveFrame=True) # 自动保存帧
webcam_opencv(is_autoSaveFrame=True, is_compress=True) # 压缩帧
```

❗ 注意：`==`后面一定要加**版本号**。如果执行`pip install opencv-webcam-script` 可能安装为测试版，会影响使用。



### ✅ 方法三：docker 镜像安装（docker版）

#### 🔥 [opencv-webcam-script docker版 详细教程](./tutorial/ows_docker.md)

#### 🎨 简易教程

📌 第一步：下载镜像

```shell
sudo docker pull zengdockerdocker/opencv-webcam-script:v0.5 # 镜像拉取
```

📌 第二步：创建容器

```shell
xhost +local:root # 允许root用户访问正在运行的X服务器（重要）
# 创建名称为ows的容器
sudo docker run --name=ows --ipc=host -it -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=unix$DISPLAY --device=/dev/video0:/dev/video0 zengdockerdocker/opencv-webcam-script:v0.5
```

📌 第三步：运行程序

```shell
python3 opencv_webcam.py # 运行脚本程序
python3 opencv_webcam.py -isasf # 自动保存帧

# 获取docker帧数据到本机
sudo docker cp 容器名称:容器目录 本机宿主目录
sudo docker cp ows:/usr/src/app/WebcamFrame /home/用户名 # 举例
```

❗ 注意：启动ows容器前，需要执行`xhost +local:root` 指令，确保连接到本地设备。



### ✅ 方法四：Podman 安装（Podman版）

#### 🔥 [opencv-webcam-script Podman版 详细教程](./tutorial/ows_podman.md)

#### 🎨 简易教程

📌 第一步：下载镜像

```shell
sudo podman pull docker.io/zengdockerdocker/opencv-webcam-script:v0.5 # 镜像拉取
```

📌 第二步：创建容器

```shell
xhost +local:root # 允许root用户访问正在运行的X服务器（重要）
# 创建名称为ows的容器
sudo podman run --name=ows --ipc=host -it -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=unix$DISPLAY --device=/dev/video0:/dev/video0 docker.io/zengdockerdocker/opencv-webcam-script:v0.5
```

📌 第三步：运行程序

```shell
python3 opencv_webcam.py # 运行脚本程序
python3 opencv_webcam.py -isasf # 自动保存帧
```

❗ 注意：启动ows容器前，需要执行`xhost +local:root` 指令，确保连接到本地设备。



### ✅ 方法五：Jupyter 安装（Jupyter版）

#### 🔥 [opencv-webcam-script Jupyter版 详细教程](./tutorial/ows_jupyter.ipynb)

#### 🎨 简易教程

📌 第一步：安装

```shell
# 首先手动创建虚拟环境
# conda create -n ows python==3.8 # 虚拟环境名称为ows
# conda activate ows # 激活虚拟环境
!git clone https://gitee.com/CV_Lab/opencv_webcam.git  # 克隆
%cd opencv_webcam
%pip install -qr requirements.txt -U  # 安装
```

📌 第二步：运行

```shell
!python opencv_webcam.py # 默认按q键退出
!python opencv_webcam.py -isasf # 视频帧自动保存
```

**注：[Jupyter版ows](https://gitee.com/CV_Lab/opencv_webcam/blob/master/tutorial/ows_jupyter.ipynb)程序通过`Ctrl+Enter` 即可运行单元指令**





<h2 align="center">⚡使用教程</h2>

<p align="center" >
<img src="https://pic.imgdb.cn/item/620f06da2ab3f51d9154029e.png">
</p>

####  💡 常规启动

```shell
# 启动ows程序
python opencv_webcam.py
```

❗ 提示：启动ows程序后，首先进入**倒计时模式**，有**150帧**倒计时准备时间，会提示用户将设备调整到合适的位置，准备开始。



####  💡 设备选择

```shell
# 多摄像头切换示例，默认为0
python opencv_webcam.py -dev 0
python opencv_webcam.py -dev 1
python opencv_webcam.py -dev 2

# RTSP
python opencv_webcam.py -dev rtsp://username:password@xxx.xxx.xxx
```



####  💡 设置退出键

```shell
# 默认按q键退出
python opencv_webcam.py -q z # 设置z键退出
python opencv_webcam.py -q k # 设置k键退出
```

<p align="center" >
<img src="https://pic.imgdb.cn/item/6205b37d2ab3f51d91f91322.png">
</p>


####  💡 自动保存帧

```shell
python opencv_webcam.py -isasf
```



#### 💡 每隔n帧保存一次帧

```shell
# 每隔10帧保存一次帧
python opencv_webcam.py -isasf -fns 10
```



#### 💡 手动保存帧

```shell
# 默认按a键捕获一帧
python opencv_webcam.py -ishsf
```



#### 💡 自定义捕获键

```shell
# 设置z键为捕获键，默认为a键
python opencv_webcam.py -ishsf -fck z
```



#### 💡 重塑帧尺寸（自定义宽高）

```shell
# 重塑宽度300 高度200
python opencv_webcam.py -isasf -isrf -rf 300,200 # 自动版
python opencv_webcam.py -ishsf -isrf -rf 300,200 # 手动版
```



#### 💡 重塑帧尺寸（自定义宽高缩放比）

```shell
# 宽高缩放比为0.5
python opencv_webcam.py -isasf -isrf -rrf 0.5 # 自动版
python opencv_webcam.py -ishsf -isrf -rrf 0.5 # 手动版
```



#### 💡 自定义保存路径

```shell
# 设置保存路径，默认保存路径为./WebcamFrame
python opencv_webcam.py -fsd custom_dir -isasf # 以自动版为例
```



####  💡 自定义帧目录名称

```shell
# 设置帧目录名称，默认保存路径为frames
python opencv_webcam.py -fdn frames_custom -isasf # 以自动版为例
```




####  💡 自定义帧名前缀

```shell
# 设置帧图片的前缀名称
python opencv_webcam.py -isasf -fnp webcam # 以自动版为例
```



#### 💡 设置帧保存格式

```shell
# 设置JPG质量为100，默认为95
python opencv_webcam.py -isasf -fss jpg -jq 100

# 设置PNG质量为5，默认为3
python opencv_webcam.py -isasf -fss png -pq 5
```



#### 💡 设置暂停键

```shell
# 设置w键为暂停键，默认为p键
python opencv_webcam.py -p w # 按任意键继续
```



#### 💡 设置保存帧数

```shell
# 设置保存100帧
python opencv_webcam.py -isasf -afn 100
```

<p align="center" >
<img src="https://pic.imgdb.cn/item/6205b37f2ab3f51d91f914ec.png">
</p>


#### 💡 日志设置

```shell
# 日志文件默认保存在项目根目录，ows.log
python opencv_webcam.py

# 设置日志文件名称及类型，类型包括.log、.txt、.data
python opencv_webcam.py -ln ows02.txt

# 设置日志保存方式，，默认为追加模式
python opencv_webcam.py -lm w # 设置为覆盖模式
```

 <p align="center" >
<img src="https://pic.imgdb.cn/item/6205b37b2ab3f51d91f910f8.png">
</p>


#### 💡 视频帧压缩

```shell
# 常规压缩，默认ows.zip（以自动版为例）
python opencv_webcam.py -isasf -isc

# 自定义压缩文件名称
python opencv_webcam.py -isasf -isc -cn ows02

# 自动命名压缩文件
python opencv_webcam.py -isasf -isc -isacn

# 自定义压缩模式，默认为写覆盖
python opencv_webcam.py -isasf -isc -cs tar -cm w:gz # tar压缩
python opencv_webcam.py -isasf -isc -cm a # 追加模式(a模式仅限zip) 注：该指令仅限v0.4
```



#### 💡 日期-帧数图

该功能在每次运行OWS程序时，根据`date_time_frames.csv`文件会自动生成**日期-帧数图**。

<p align="center" >
<img src="https://pic.imgdb.cn/item/61f520d12ab3f51d915457e4.png">
</p>




#### 💡 指令查询

```shell
# 查询脚本参数
python opencv_webcam.py --help
```

<details open>
<summary>指令查询结果</summary>

```shell
usage: opencv_webcam.py [-h] [--device DEVICE] [--quit QUIT] [--is_autoSaveFrame] [--is_handSaveFrame] [--is_resizeFrame] [--frame_saveDir FRAME_SAVEDIR] [--frame_dirName FRAME_DIRNAME]
                        [--frame_nSave FRAME_NSAVE] [--frame_capKey FRAME_CAPKEY] [--resize_frame RESIZE_FRAME] [--resizeRatio_frame RESIZERATIO_FRAME] [--frame_namePrefix FRAME_NAMEPREFIX]
                        [--frame_saveStyle FRAME_SAVESTYLE] [--jpg_quality JPG_QUALITY] [--png_quality PNG_QUALITY] [--pause PAUSE] [--auto_frameNum AUTO_FRAMENUM] [--logName LOGNAME]
                        [--logMode LOGMODE] [--is_compress] [--compressStyle COMPRESSSTYLE] [--is_autoCompressName] [--compressName COMPRESSNAME] [--compressMode COMPRESSMODE]

OpenCV Webcam Script v0.5

optional arguments:
  -h, --help            show this help message and exit
  --device DEVICE, -dev DEVICE
                        device index for webcam, 0 or rtsp
  --quit QUIT, -q QUIT  quit key for webcam
  --is_autoSaveFrame, -isasf
                        is auto save frame
  --is_handSaveFrame, -ishsf
                        is hand save frame
  --is_resizeFrame, -isrf
                        is resize frame
  --frame_saveDir FRAME_SAVEDIR, -fsd FRAME_SAVEDIR
                        save frame dir
  --frame_dirName FRAME_DIRNAME, -fdn FRAME_DIRNAME
                        save frame dir name
  --frame_nSave FRAME_NSAVE, -fns FRAME_NSAVE
                        n frames save a frame (auto save frame)
  --frame_capKey FRAME_CAPKEY, -fck FRAME_CAPKEY
                        frame capture key (hand save frame)
  --resize_frame RESIZE_FRAME, -rf RESIZE_FRAME
                        resize frame save
  --resizeRatio_frame RESIZERATIO_FRAME, -rrf RESIZERATIO_FRAME
                        resize ratio frame save
  --frame_namePrefix FRAME_NAMEPREFIX, -fnp FRAME_NAMEPREFIX
                        frame name prefix
  --frame_saveStyle FRAME_SAVESTYLE, -fss FRAME_SAVESTYLE
                        frame save style
  --jpg_quality JPG_QUALITY, -jq JPG_QUALITY
                        frame save jpg quality (0-100) default 95
  --png_quality PNG_QUALITY, -pq PNG_QUALITY
                        frame save jpg quality (0-9) default 3
  --pause PAUSE, -p PAUSE
                        webcam pause
  --auto_frameNum AUTO_FRAMENUM, -afn AUTO_FRAMENUM
                        auto save number of frames
  --logName LOGNAME, -ln LOGNAME
                        log save name
  --logMode LOGMODE, -lm LOGMODE
                        log write mode
  --is_compress, -isc   is compress file
  --compressStyle COMPRESSSTYLE, -cs COMPRESSSTYLE
                        compress style
  --is_autoCompressName, -isacn
                        is auto compress name
  --compressName COMPRESSNAME, -cn COMPRESSNAME
                        compress save name
  --compressMode COMPRESSMODE, -cm COMPRESSMODE
                        compress save mode, tar w:gz
```

</details>
