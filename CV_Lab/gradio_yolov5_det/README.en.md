[中文简体](./README.md)|English

<p align="center">
<a href="https://gitee.com/CV_Lab/gradio_yolov5_det">
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yolov5_det_logo.png" alt="Simple Icons" >
</a>
<p align="center">
    Gradio-based YOLOv5 general object detection demonstration system
</p>
<p align="center">
    Customizable detection model, easy demonstration, easy installation
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



<h2 align="center">🚀 Update Trend</h2>

- `2022-04-30` **⚡ [Gradio YOLOv5 Det v0.1](https://gitee.com/CV_Lab/gradio_yolov5_det/releases/v0.1)officially launched**

<h2 align="center">🤗 Online demo</h2>

❤️ This project provides an **online demo**, click the logo below to enter the **Hugging Face Spaces** for a quick experience:

<div align="center" >
<a href="https://huggingface.co/spaces/Zengyf-CVer/gradio_yolov5_det">
<img src="https://pycver.gitee.io/ows-pics/imgs/huggingface_logo.png">
</a>
</div>
<h2 align="center">💎 Project Process and Purpose</h2>

### 📌 The overall process of the project

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gyd_workflow.png">
</div>


### 📌 Project example

#### ❤️ Detection interface and effect 01

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yd_01.png">
</div>

#### ❤️ Detection interface and effect 02: Detection information

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yd_09.png">
</div>
<div align="center" >
Detection interface
</div>


<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yd_10.png">
</div>
<div align="center" >
Detection information (JSON version)
</div>


#### ❤️ Functional interface

Including **image upload**, **device switching**, **model switching**, **inference size**, **confidence threshold**, **IoU threshold**, **label display** , **category selection**, etc.

❗ Note: The position of the slider of Gradio's slider component does not change with the default value initially, but it does not affect the detection

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gyd_ui01.png">
</div>
<div align="center" >
Function interface 01
</div>


<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gyd_ui02.png">
</div>
<div align="center" >
Function interface 02
</div>


<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gyd_ui03.png">
</div>
<div align="center" >
Function interface 03
</div>


#### ❤️ Quick experience

This project provides three **examples**, users can quickly experience the detection effect

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yd_examples.png">
</div>
<div align="center" >
Sample interface
</div>

<h2 align="center">💡 Project Structure</h2>

```
.
├── gradio_yolov5_det						# project name
│   ├── yolov5								# YOLOv5 project
│   ├── model_download						# model download
│   │   ├── yolov5_model_p5_all.sh			# YOLOv5 P5 model
│   │   ├── yolov5_model_p6_all.sh			# YOLOv5 P6 model
│   │   └── yolov5_model_p5_n.sh			# yolov5n model
│   ├── model_config						# model configuration
│   │   ├── model_name_p5_all.yaml			# YOLOv5 P5 model name (yaml version)
│   │   ├── model_name_p6_all.yaml			# YOLOv5 P6 model name (yaml version)
│   │   ├── model_name_p5_n.yaml			# yolov5n model name (yaml version)
│   │   ├── model_name_p5_all.csv			# YOLOv5 P5 model name (csv version)
│   │   ├── model_name_p6_all.csv			# YOLOv5 P6 model name (csv version)
│   │   └── model_name_p5_n.csv				# yolov5n model name (csv version)
│   ├── cls_name							# class name
│   │   ├── cls_name.yaml					# Category name file (yaml version)
│   │   ├── cls_name_en.yaml				# Category English name file (yaml version)
│   │   └── cls_name.csv					# Category name file (csv version)
│   ├── img_example							# example image
│   ├── __init__.py							# Initialization file
│   ├── gradio_yolov5_det.py				# Main run file
│   ├── LICENSE								# Project License
│   ├── CodeCheck.md						# Code Check
│   ├── .gitignore							# git ignore files
│   ├── yolov5_pytorch_gpu.md				# YOLOv5 PyTorch GPU installation tutorial
│   ├── README.md							# Project description
│   ├── README.en.md						# Project description (English version)
│   └── requirements.txt					# Script dependencies
```

<h2 align="center">🔥 Installation Tutorial</h2>

### ✅ Step 1: Install Gradio YOLOv5 Det

📌 Create a conda environment

```shell
conda create -n yolo python==3.8
conda activate yolo # enter the environment
````

📌 recursive clone

```shell
git clone https://gitee.com/CV_Lab/gradio_yolov5_det.git --recursive
````

📌 Install Gradio YOLOv5 Det dependencies

```shell
pip install -r ./requirements.txt -U
````

### ✅ Step 2: Install YOLOv5 dependencies

```shell
conda activate facelabel # enter the environment
cd ./face-labeling/yolov5
pip install -r requirements.txt -U
````

❗ Note: yolov5 uses pip to install the PyTorch GPU version by default. If you use the official website to install the **PyTorch GPU** version, see [YOLOv5 PyTorch GPU Installation Tutorial](./yolov5_pytorch_gpu.md)

<h2 align="center">⚡ Usage</h2>

### 💡 Running the YOLOv5 model

📌 Download YOLOv5 P5 model

```shell
# Download yolov5n.pt, yolov5s.pt, yolov5m.pt, yolov5l.pt, yolov5x.pt to the yolov5 directory
bash ./model_download/yolov5_model_p5_all.sh
````

📌 run

```shell
python gradio_yolov5_det.py
# Enter in the browser: http://127.0.0.1:7860/ or http://127.0.0.1:7861/ and so on (observe the shell prompt for details)
````

❗ Note: This project provides 3 YOLOv5 model download scripts, the default is `yolov5_model_p5_all.sh`

```shell
# yolov5n model download and run
bash ./model_download/yolov5_model_p5_n.sh
python gradio_yolov5_det.py -mc ./model_config/model_name_p5_n.yaml (yaml version)
# python gradio_yolov5_det.py -mc ./model_config/model_name_p5_n.csv (csv version)

# YOLOv5 P6 model download and run
bash ./model_download/yolov5_model_p6_all.sh
python gradio_yolov5_det.py -mc ./model_config/model_name_p6_all.yaml (yaml version)
# python gradio_yolov5_det.py -mc ./model_config/model_name_p6_all.csv (csv version)
````

❗ Note: Default class file [cls_name.yaml](./cls_name/cls_name.yaml)|[cls_name.csv](./cls_name/cls_name.csv)

### 💡 Running custom models

```shell
python gradio_yolov5_det.py -mc custom_model_name.yaml -cls custom_model_cls_name.yaml (yaml version)
# python gradio_yolov5_det.py -mc custom_model_name.csv -cls custom_model_cls_name.csv (csv version)

# Enter in the browser: http://127.0.0.1:7860/ or http://127.0.0.1:7861/ and so on (observe the shell prompt for details)
````

📌 Custom model name

`custom_model_name.yaml` format:

````yaml
model_names: ["widerface-s", "widerface-m", "widerface-l"]
````

`custom_model_name.csv` format (`\n` delimited):

```shell
widerface-s
widerface-m
widerface-l
````

📌 Custom model categories

`custom_model_cls_name.yaml` format:

````yaml
model_cls_name: ["face"]
````

`custom_model_cls_name.csv` format (`\n` separated):

```shell
face
````

### 💡 Other operations

```shell
# device switching (cuda or cpu)
python gradio_yolov5_det.py -dev 0 # cuda 0
python gradio_yolov5_det.py -dev cpu #cpu

# Customize the default model name of the drop-down box
python gradio_yolov5_det.py -mn yolov5m

# Custom NMS confidence threshold
python gradio_yolov5_det.py -conf 0.8

# Custom NMS IoU threshold
python gradio_yolov5_det.py -iou 0.5

# Set the detection label not to be displayed by default
python gradio_yolov5_det.py -lds
````

### 💡 Interface operation

Operate drop-down boxes, sliders, and checkboxes to make relevant settings for the model

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gyd_opt01.png">
</div>

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gyd_opt02.png">
</div>

Manipulate the checkbox group to set the model category

Click the `Submit` button to check the function, click the `clear` button to reset all settings

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yd_05.png">
</div>

Click the `Flag` button to download the relevant detection data

<div align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/gradio_yd_08.png">
</div>
❗ Note: The above interface is for reference only, the specific interface is mainly based on the actual project.

The relevant detection data is stored in the `run` directory with the following structure:

````
# run directory
.
├── run						# face data storage directory
│   ├── 原始图片				# Original image catalog
│   │   ├── 0.jpg			# Original picture
│   │   ├──	1.jpg			# Original picture
│   │   ├──	......
│   ├── 检测图片				# Detect picture directory
│   │   ├── 0.png			# Detection picture
│   │   ├── 1.png			# Detection picture
│   │   ├──	......
│   ├── log.csv				# Detection log
````
