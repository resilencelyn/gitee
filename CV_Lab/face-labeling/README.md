中文简体|[English](./README.en.md)

<p align="center">
<a href="https://gitee.com/CV_Lab/face-labeling">
<img src="https://pycver.gitee.io/ows-pics/imgs/facelabeling_logo.png" alt="Simple Icons" >
</a>
<p align="center">
    基于YOLOv5的智能人脸数据标注工具，实现人脸数据标注自动化
</p>
<p align="center">
    可自定义人脸检测模型、可导出多种格式标签，包括PASCAL VOC XML、MS COCO JSON、YOLO TXT
</p>
</p>
<p align="center">
<a href="./CodeCheck.md"><img src="https://img.shields.io/badge/CodeCheck-passing-success" alt="code check" /></a>
<a href="https://github.com/ultralytics/yolov5"><img src="https://img.shields.io/badge/YOLOv5-v6.1-blue" alt="YOLOv5 Version" /></a>
<a href="https://gitee.com/CV_Lab/yolov5_rt_tfjs/blob/master/LICENSE"><img src="https://img.shields.io/badge/License-GPL--3.0-blue" alt="License" /></a>
</p>
<p align="center">
<a href="#"><img src="https://img.shields.io/badge/Python-3.8%2B-blue?logo=python" alt="Python Version" /></a>
<a href="http://host.robots.ox.ac.uk/pascal/VOC/"><img src="https://img.shields.io/badge/PASCAL%20VOC-XML-blue" alt="PASCAL VOC XML" /></a>
<a href="https://cocodataset.org/"><img src="https://img.shields.io/badge/MS%20COCO-JSON-brightgreen" alt="MS COCO JSON" /></a>
<a href="https://pjreddie.com/darknet/"><img src="https://img.shields.io/badge/YOLO-TXT-orange" alt="YOLO TXT" /></a>
</p>



<h2 align="center">🚀更新走势</h2>



<h2 align="center">💎项目流程与用途</h2>

### 📌 项目整体流程与扩展应用

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/face_labeling_work_flow.png">
</div>

### 📌 项目功能结构与信息流

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/face_labeling_work_io.png">
</div>




<h2 align="center">💡项目结构</h2>

```
.
├── face-labeling							# 项目名称
│   ├── yolov5								# YOLOv5项目
│   ├── util								# 工具包
│   │   ├── voc_xml.py						# PASCAL VOC XML
│   │   ├── coco_json.py					# MS COCO JSON
│   │   ├── yolo_txt.py						# YOLO TXT
│   │   ├── model_opt.py					# 模型管理
│   │   ├── obj_opt.py						# 目标管理
│   │   ├── path_opt.py						# 路径管理
│   │   ├── log.py							# 日志管理
│   │   └── time_format.py					# 日期格式化
│   ├── data								# 测试数据
│   │   └── imgs							# 测试图片，来源于WIDER FACE Test
│   ├── face_labeling.py					# 主运行文件
│   ├── LICENSE								# 项目许可
│   ├── CodeCheck.md						# 代码检查
│   ├── .gitignore							# git忽略文件
│   ├── yolov5_widerface.md					# 基于YOLOv5的人脸检测模型的构建
│   ├── yolov5_pytorch_gpu.md				# YOLOv5 PyTorch GPU安装教程
│   ├── README.md							# 项目说明
│   ├── README.en.md						# 项目说明（英文版）
│   └── requirements.txt					# 脚本依赖包
```



<h2 align="center">🔥安装教程</h2>

### ✅ 第一步：安装Face Labeling

📌 创建conda环境

```shell
conda create -n facelabel python==3.8
conda activate facelabel # 进入环境
```

📌 递归式克隆

```shell
git clone https://gitee.com/CV_Lab/face-labeling.git --recursive
```

📌 将人脸模型文件（.pt）放入`yolov5` 目录中



### ✅ 第二步：安装YOLOv5依赖

```shell
conda activate facelabel # 进入环境
cd ./face-labeling/yolov5
pip install -r requirements.txt -U
```

❗ 注意：yolov5默认采用pip安装PyTorch GPU版，如果采用官网安装**PyTorch GPU**版，参见[YOLOv5 PyTorch GPU安装教程](./yolov5_pytorch_gpu.md)



### ✅ [基于YOLOv5的人脸检测模型的构建](./yolov5_widerface.md)

本项目使用的**人脸检测模型**是在[WIDER FACE](http://shuoyang1213.me/WIDERFACE/)数据集上，基于[YOLOv5 v6.1](https://github.com/ultralytics/yolov5)训练的，具体训练过程参见[yolov5_widerface.md](./yolov5_widerface.md)

本项目提供了以下人脸检测模型：

|  模型名称   |                           下载地址                           |
| :---------: | :----------------------------------------------------------: |
| widerface-m | [百度云](https://pan.baidu.com/s/19cIqLc05EbyxxEVMX7Lr3g) \| 提取码：5gfs |




<h2 align="center">⚡使用教程</h2>

### 💡 webcam实时标注

```shell
# a键捕获视频帧，q键退出
python face_labeling.py
```



### 💡 图片标注（包括批量图片标注）

```shell
python face_labeling.py -m img # 默认测试图片目录data/imgs
python face_labeling.py -m img -imd ./img_dir # 指定图片目录
```



### 💡 视频标注（包括批量视频标注）

```shell
python face_labeling.py -m video # 默认测试视频目录data/videos
python face_labeling.py -m video -vd ./video_dir # 指定视频目录
```

❗ 说明：以上三种检测模式都会在项目根目录中生成`FaceFrame`目录，该目录会生成`frame*`的子目录，子目录结构如下：

```
# webcam和图片标注的目录
.
├── FaceFrame						# 人脸数据保存目录
│   ├── frame						# 子目录
│   │   ├── raw						# 原始图片
│   │   ├── tag						# 标记图片（包括：人脸检测框、人脸ID、置信度、帧ID、FPS、人脸总数，人脸尺寸类型（小、中、大）数量）
│   │   ├── voc_xml					# PASCAL VOC XML 标注文件
│   │   ├── coco_json				# MS COCO JSON 标注文件
│   │   ├── yolo_txt				# YOLO TXT 标注文件
│   ├── frame2						# 子目录
│   │   ├── raw						# 原始图片
│   │   ├── ......
```



```
# 视频标注的目录
.
├── FaceFrame						# 人脸数据保存目录
│   ├── frame						# 子目录
│	│   ├── video_name01			# 子视频目录
│   │   │   ├── raw					# 原始图片
│   │   │   ├── tag					# 标记图片（包括：人脸检测框、人脸ID、置信度、帧ID、FPS、人脸总数，人脸尺寸类型（小、中、大）数量）
│   │   │   ├── voc_xml				# PASCAL VOC XML 标注文件
│   │   │   ├── coco_json			# MS COCO JSON 标注文件
│   │   │   ├── yolo_txt			# YOLO TXT 标注文件
│	│   ├── video_name02			# 子视频目录
│   │   │   ├── raw					# 原始图片
│   │   │   ├── ......
```


❗ 查看检测结果：人脸图片检测结果会保存在`FaceFrame/frame*/tag`中，以`python face_labeling.py -m img`为例运行项目自带检测图片，检测结果如下：

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/widerface_m_img.jpg">
</div>

<div align="center" >
widerface-m检测结果
</div>



### 💡 自定义人脸模型

```shell
# 默认为widerface-m
python face_labeling.py -mn face_model # 以实时标注为例
```



### 💡 自定义类别

```shell
# 默认为face，以口罩识别为例
python face_labeling.py -cls mask # 口罩类
python face_labeling.py -cls without-mask # 未戴口罩类
```



### 💡 自定义模型参数

```shell
# 可以根据自定义人脸模型进行相应的调参，以实时标注为例

# NMS 置信度阈值，默认为0.5
python face_labeling.py -conf 0.8

# NMS IoU阈值，默认为0.45
python face_labeling.py -iou 0.5

# 单张图片的最大检测目标数，默认为1000
python face_labeling.py -mdn 10

# 以上参数也可以同时使用，例如：
python face_labeling.py -conf 0.8 -iou 0.5
python face_labeling.py -conf 0.8 -iou 0.5 -mdn 10

# 模型推理尺寸
python face_labeling.py -isz 320
```



### 💡 自定义保存目录名称

```shell
# 默认为FaceFrame
python face_labeling.py -fsd face_dir # 以实时标注为例
```



### 💡 自定义保存子目录名称

```shell
# 默认为frame
python face_labeling.py -fdn face_subDir # 以实时标注为例
```



### 💡 自定义图片前缀

```shell
# 默认为face_test
python face_labeling.py -in face # 以实时标注为例
```

