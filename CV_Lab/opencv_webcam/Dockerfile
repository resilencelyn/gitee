# Opencv Webcam Script v0.5, GPL-3.0 License
# 创建人：曾逸夫
# 创建时间：2022-02-06


FROM ubuntu:latest
LABEL maintainer="zyfiy1314@163.com"

# 安装linux包
RUN apt update
RUN DEBIAN_FRONTEND=noninteractive TZ=Asia/Shanghai apt-get -y install tzdata
RUN apt install -y python3-pip zip htop screen libgl1-mesa-glx libgtk2.0-dev vim wget
# 解决vim中文乱码问题
RUN echo "set enc=utf8" > ~/.vimrc

# 安装依赖
COPY requirements.txt .
RUN python3 -m pip install --no-cache --upgrade pip
RUN pip install --no-cache -U -r requirements.txt

# 创建工作目录
RUN mkdir -p /usr/src/app
WORKDIR /usr/src/app

# 复制内容
COPY . /usr/src/app
