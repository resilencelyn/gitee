中文简体|[English](./README.en.md)

<p align="center">
<a href="https://gitee.com/CV_Lab/gradio_yolov5_det">
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yolov5_det_logo.png" alt="Simple Icons" >
</a>
<p align="center">
    基于Gradio的YOLOv5通用目标检测演示系统
</p>
<p align="center">
    可自定义检测模型、演示便捷、安装简单
</p>
</p>
<p align="center">
<a href="./CodeCheck.md"><img src="https://img.shields.io/badge/CodeCheck-passing-success" alt="code check" /></a>
<a href="https://gitee.com/CV_Lab/gradio_yolov5_det/releases/v0.1"><img src="https://img.shields.io/badge/Releases-v0.1-green" alt="Releases Version" /></a>
<a href="https://github.com/ultralytics/yolov5"><img src="https://img.shields.io/badge/YOLOv5-v6.1-blue" alt="YOLOv5 Version" /></a>
<a href="https://github.com/gradio-app/gradio"><img src="https://img.shields.io/badge/Gradio-2.9.4+-orange" alt="Gradio Version" /></a>
<a href="https://huggingface.co/spaces/Zengyf-CVer/gradio_yolov5_det"><img src="https://img.shields.io/badge/%F0%9F%A4%97-Hugging%20Face-blue" alt="Hugging Face Spaces" /></a>
<a href="https://gitee.com/CV_Lab/gradio_yolov5_det/blob/master/LICENSE"><img src="https://img.shields.io/badge/License-GPL--3.0-blue" alt="License" /></a>
</p>
<p align="center">
<a href="#"><img src="https://img.shields.io/badge/Python-3.8%2B-blue?logo=python" alt="Python Version" /></a>
<a href="https://pycqa.github.io/isort/"><img src="https://img.shields.io/badge/%20imports-isort-%231674b1?style=flat&labelColor=ef8336" alt="Imports: isort"></a>
<a href="https://github.com/PyCQA/flake8"><img src="https://img.shields.io/badge/PyCQA-flake8-brightgreen" alt="flake8"></a>
    <a href="https://github.com/executablebooks/mdformat"><img src="https://img.shields.io/badge/markdown-mdformat-blue" alt="markdown format"></a>
	<a href="https://github.com/pyupio/safety"><img src="https://img.shields.io/badge/dependencies-Safety-blue" alt="Safety"></a>
</p>

<h2 align="center">🚀更新走势</h2>

- `2022-04-30` **⚡ [Gradio YOLOv5 Det v0.1](https://gitee.com/CV_Lab/gradio_yolov5_det/releases/v0.1)正式上线**

<h2 align="center">🤗在线demo</h2>

❤️ 本项目提供了**在线demo**，点击下面的logo，进入**Hugging Face Spaces**中快速体验：

<div align="center" >
<a href="https://huggingface.co/spaces/Zengyf-CVer/gradio_yolov5_det">
<img src="https://pycver.gitee.io/ows-pics/imgs/huggingface_logo.png">
</a>
</div>

### 💡 Demo 列表

|                          Demo 名称                           | 输入类型 |  输出类型  |                             状态                             |
| :----------------------------------------------------------: | :------: | :--------: | :----------------------------------------------------------: |
| [Gradio YOLOv5 Det v0.1](https://huggingface.co/spaces/Zengyf-CVer/gradio_yolov5_det) |   图片   | 图片，JSON | [![demo status](https://img.shields.io/website-up-down-green-red/https/hf.space/gradioiframe/Zengyf-CVer/gradio_yolov5_det/+.svg?label=demo%20status)](https://huggingface.co/spaces/Zengyf-CVer/gradio_yolov5_det) |
| [Gradio YOLOv5 Det v0.2](https://huggingface.co/spaces/Zengyf-CVer/Gradio_YOLOv5_Det_v2) |   图片   | 图片，JSON | [![demo status](https://img.shields.io/website-up-down-green-red/https/hf.space/gradioiframe/Zengyf-CVer/Gradio_YOLOv5_Det_v2/+.svg?label=demo%20status)](https://huggingface.co/spaces/Zengyf-CVer/Gradio_YOLOv5_Det_v2) |



<h2 align="center">💎项目流程与用途</h2>

### 📌 项目整体流程

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gyd_workflow.png">
</div>


### 📌 项目示例

#### ❤️ 检测界面与效果01

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yd_01.png">
</div>

#### ❤️ 检测界面与效果02：检测信息

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yd_09.png">
</div>

<div align="center" >
检测界面
</div>

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yd_10.png">
</div>

<div align="center" >
检测信息（JSON版）
</div>

#### ❤️ 功能界面

包括**图片上传**、**设备切换**、**模型切换**、**推理尺寸**、**置信度阈值**、**IoU阈值**、**标签显示**、**类别选择**等

❗ 注：Gradio的滑动条组件的滑块的位置，初始时不跟随默认值变化，但不影响检测

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gyd_ui01.png">
</div>

<div align="center" >
功能界面01
</div>

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gyd_ui02.png">
</div>

<div align="center" >
功能界面02
</div>

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gyd_ui03.png">
</div>

<div align="center" >
功能界面03
</div>

#### ❤️ 快速体验

本项目提供了三个**示例**，用户可以快速体验检测效果：

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yd_examples.png">
</div>

<div align="center" >
示例界面
</div>

<h2 align="center">💡项目结构</h2>

```
.
├── gradio_yolov5_det						# 项目名称
│   ├── yolov5								# YOLOv5项目
│   ├── model_download						# 模型下载
│   │   ├── yolov5_model_p5_all.sh			# YOLOv5 P5模型
│   │   ├── yolov5_model_p6_all.sh			# YOLOv5 P6模型
│   │   └── yolov5_model_p5_n.sh			# yolov5n模型
│   ├── model_config						# 模型配置
│   │   ├── model_name_p5_all.yaml			# YOLOv5 P5 模型名称（yaml版）
│   │   ├── model_name_p6_all.yaml			# YOLOv5 P6 模型名称（yaml版）
│   │   ├── model_name_p5_n.yaml			# yolov5n 模型名称（yaml版）
│   │   ├── model_name_p5_all.csv			# YOLOv5 P5 模型名称（csv版）
│   │   ├── model_name_p6_all.csv			# YOLOv5 P6 模型名称（csv版）
│   │   └── model_name_p5_n.csv				# yolov5n 模型名称（csv版）
│   ├── cls_name							# 类别名称
│   │   ├── cls_name.yaml					# 类别名称文件（yaml版）
│   │   ├── cls_name_en.yaml				# 类别英文名称文件（yaml版）
│   │   └── cls_name.csv					# 类别名称文件（csv版）
│   ├── img_example							# 示例图片
│   ├── __init__.py							# 初始化文件
│   ├── gradio_yolov5_det.py				# 主运行文件
│   ├── LICENSE								# 项目许可
│   ├── CodeCheck.md						# 代码检查
│   ├── .gitignore							# git忽略文件
│   ├── yolov5_pytorch_gpu.md				# YOLOv5 PyTorch GPU安装教程
│   ├── README.md							# 项目说明
│   ├── README.en.md						# 项目说明（英文版）
│   └── requirements.txt					# 脚本依赖包
```

<h2 align="center">🔥安装教程</h2>

### ✅ 第一步：安装Gradio YOLOv5 Det

📌 创建conda环境

```shell
conda create -n yolo python==3.8
conda activate yolo # 进入环境
```

📌 递归式克隆

```shell
git clone https://gitee.com/CV_Lab/gradio_yolov5_det.git --recursive
```

📌 安装Gradio YOLOv5 Det依赖

```shell
pip install -r ./requirements.txt -U
```

### ✅ 第二步：安装YOLOv5依赖

```shell
conda activate facelabel # 进入环境
cd ./face-labeling/yolov5
pip install -r requirements.txt -U
```

❗ 注意：yolov5默认采用pip安装PyTorch GPU版，如果采用官网安装**PyTorch GPU**版，参见[YOLOv5 PyTorch GPU安装教程](./yolov5_pytorch_gpu.md)

<h2 align="center">⚡使用教程</h2>

### 💡 运行YOLOv5模型

📌 下载YOLOv5 P5模型

```shell
# 将yolov5n.pt,yolov5s.pt,yolov5m.pt,yolov5l.pt,yolov5x.pt下载到yolov5目录中
bash ./model_download/yolov5_model_p5_all.sh
```

📌 运行

```shell
python gradio_yolov5_det.py
# 在浏览器中输入：http://127.0.0.1:7860/或者http://127.0.0.1:7861/ 等等（具体观察shell提示）
```

❗ 注：本项目提供了3种YOLOv5模型下载脚本，默认为`yolov5_model_p5_all.sh`

```shell
# yolov5n模型下载及运行
bash ./model_download/yolov5_model_p5_n.sh
python gradio_yolov5_det.py -mc ./model_config/model_name_p5_n.yaml （yaml版）
# python gradio_yolov5_det.py -mc ./model_config/model_name_p5_n.csv （csv版）

# YOLOv5 P6模型下载及运行
bash ./model_download/yolov5_model_p6_all.sh
python gradio_yolov5_det.py -mc ./model_config/model_name_p6_all.yaml （yaml版）
# python gradio_yolov5_det.py -mc ./model_config/model_name_p6_all.csv （csv版）
```

❗ 注：默认类别文件[cls_name.yaml](./cls_name/cls_name.yaml)|[cls_name.csv](./cls_name/cls_name.csv)

### 💡 运行自定义模型

```shell
python gradio_yolov5_det.py -mc custom_model_name.yaml -cls custom_model_cls_name.yaml （yaml版）
# python gradio_yolov5_det.py -mc custom_model_name.csv -cls custom_model_cls_name.csv （csv版）

# 在浏览器中输入：http://127.0.0.1:7860/或者http://127.0.0.1:7861/ 等等（具体观察shell提示）
```

📌 自定义模型名称

`custom_model_name.yaml`格式：

```yaml
model_names: ["widerface-s", "widerface-m", "widerface-l"]
```

`custom_model_name.csv`格式（`\n`分隔）：

```shell
widerface-s
widerface-m
widerface-l
```

📌 自定义模型类别

`custom_model_cls_name.yaml`格式：

```yaml
model_cls_name: ["face"]
```

`custom_model_cls_name.csv`格式（`\n`分隔）：

```shell
face
```

### 💡 其他操作

```shell
# 设备切换（cuda 或者 cpu）
python gradio_yolov5_det.py -dev 0 # cuda 0
python gradio_yolov5_det.py -dev cpu # cpu

# 自定义下拉框默认模型名称
python gradio_yolov5_det.py -mn yolov5m

# 自定义NMS置信度阈值
python gradio_yolov5_det.py -conf 0.8

# 自定义NMS IoU阈值
python gradio_yolov5_det.py -iou 0.5

# 设置默认不显示检测标签
python gradio_yolov5_det.py -lds
```

### 💡 界面操作

操作下拉框、滑动条、复选框来对模型进行相关设置

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gyd_opt01.png">
</div>

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gyd_opt02.png">
</div>

操作复选框组来对模型类别进行设置

点击`Submit`按钮进行检测功能，点击`clear`按钮重置所有设置

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yd_05.png">
</div>

点击`Flag`按钮，下载相关检测数据

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yd_08.png">
</div>
❗ 注：上述界面仅供参考，具体界面以实际项目为主。

相关检测数据存储在`run`目录中，结构如下：

```
# run目录
.
├── run						# 人脸数据保存目录
│   ├── 原始图片				# 原始图片目录
│   │   ├── 0.jpg			# 原始图片
│   │   ├──	1.jpg			# 原始图片
│   │   ├──	......
│   ├── 检测图片				# 检测图片目录
│   │   ├── 0.png			# 检测图片
│   │   ├── 1.png			# 检测图片
│   │   ├──	......
│   ├── log.csv				# 检测日志
```