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

❗ 注意：启动ows容器前，需要执行`xhost +local:root` 指令，确保连接到本地设备。



#### ✨ 视频帧 podman2local

```shell
# 获取Podman帧数据到本机
sudo podman cp 容器名称:容器目录 本机宿主目录
sudo podman cp ows:/usr/src/app/WebcamFrame /home/用户名 # 举例
sudo chmod 777 -R WebcamFrame/ # 对所有用户执行读和写以及执行的权限
```



#### ✨ 其他操作

```shell
# 退出ows容器
exit

# 启动并进入ows容器
sudo podman start ows
sudo podman exec -it ows /bin/bash

# 重新下载ows镜像
# 注意：首先确保podman中无其他项目的镜像和容器
sudo podman stop ows # 关闭容器
sudo podman rm $(sudo podman ps -aq) # 删除全部容器
sudo podman rmi -f $(sudo podman images -aq) # 删除全部镜像
sudo podman pull docker.io/zengdockerdocker/opencv-webcam-script:v0.5 # 镜像拉取
```
