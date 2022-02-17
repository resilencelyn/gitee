# 🔥 opencv-webcam-script（Podman版）

## 创建人：曾逸夫



<h3 align="center">🚀 安装教程</h3>

#### 📌 第一步：下载镜像

```shell
sudo podman pull docker.io/zengdockerdocker/opencv-webcam-script:v0.5 # 镜像拉取
```



#### 📌 第二步：创建容器

```shell
xhost +local:root # 允许root用户访问正在运行的X服务器（重要）
# 创建名称为ows的容器
sudo podman run --name=ows --ipc=host -it -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=unix$DISPLAY --device=/dev/video0:/dev/video0 docker.io/zengdockerdocker/opencv-webcam-script:v0.5
```



<h3 align="center">⚡ 使用教程</h3>

#### ✨ 运行ows程序

```shell
python3 opencv_webcam.py # 运行脚本程序
python3 opencv_webcam.py -isasf # 自动保存帧
```

❤️ **其他使用方法和开发版一致，参见[README 使用教程](https://gitee.com/CV_Lab/opencv_webcam/blob/master/README.md)**



#### ✨ 视频帧 podman2local

```shell

```



#### ✨ 其他操作

```shell
# 退出ows容器
exit
```
