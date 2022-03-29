[中文简体](./README.md)|English

<p align="center">
<a href="https://gitee.com/CV_Lab/yolov5_rt_tfjs">
<img src="https://pycver.gitee.io/ows-pics/imgs/yolov5_rt_tfjs_logo.png" alt="Simple Icons" >
</a>
<p align="center">
    YOLOv5 real-time target detection project based on TensorFlow.js, supports custom models
</p>
<p align="center">
    Lightweight installation, easy to use, and strong portability
</p>
</p>
<p align="center">
<a href="./CodeCheck.md"><img src="https://img.shields.io/badge/CodeCheck-passing-success" alt="code check" /></a>
<a href="https://gitee.com/CV_Lab/yolov5_rt_tfjs/releases/v0.3"><img src="https://img.shields.io/badge/Releases-v0.3-green" alt="Releases Version" /></a>
<a href="https://github.com/ultralytics/yolov5"><img src="https://img.shields.io/badge/YOLOv5-v6.1-blue" alt="YOLOv5 Version" /></a>
<a href="https://github.com/tensorflow/tfjs"><img src="https://img.shields.io/badge/TensorFlow.js-v3.9.0+-important?logo=tensorflow" alt="TensorFlow.js Version" /></a>
<a href="https://jquery.com/"><img src="https://img.shields.io/badge/jQuery-v3.6.0+-%23436EEE?logo=jquery" alt="jQuery Version" /></a>
<a href="https://gitee.com/CV_Lab/yolov5_rt_tfjs/blob/master/LICENSE"><img src="https://img.shields.io/badge/License-GPL--3.0-blue" alt="License" /></a>
</p>
<p align="center">
<a href="./yolov5_rt_tfjs_flask"><img src="https://img.shields.io/badge/Flask-v2.0.3+-critical?logo=flask" alt="Flask Version" /></a>
<a href="./yolov5_rt_tfjs_fastapi"><img src="https://img.shields.io/badge/FastAPI-v0.74.1+-green?logo=fastapi" alt="FastAPI Version" /></a>
<a href="./yolov5_rt_tfjs_bottle"><img src="https://img.shields.io/badge/Bottle-v0.12.19%2B-blue" alt="Bottle Version" /></a>
</p>






<h2 align="center">🚀Update Trend</h2>

- `2022-03-02` **⚡ [yolov5-rt-tfjs v0.3 release version](https://gitee.com/CV_Lab/yolov5_rt_tfjs/releases/v0.3)officially launched**
- **⚡ [yolov5-rt-tfjs historical version](./v_change)**


<h2 align="center">💎Project Purpose</h2>

### 🔥 Custom Object Detection Task

<p align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/yolov5_rt_tfjs.gif">
</p>

### ❤️ Custom Model

📌 Modify [yolov5_rt_tfjs.js](./yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js) file, customize **weight file**, **category name** and **model input size**

```javascript
// weight file
const weights = '../yolov5s_web_model/model.json';

// classification name (English version)
const cls_names = ['person', 'bicycle', 'car', 'motorcycle', 'airplane', 'bus', 'train', 'truck', 'boat', 'traffic light',
    'fire hydrant', 'stop sign', 'parking meter', 'bench', 'bird', 'cat', 'dog', 'horse', 'sheep', 'cow',
    'elephant', 'bear', 'zebra', 'giraffe', 'backpack', 'umbrella', 'handbag', 'tie', 'suitcase', 'frisbee',
    'skis', 'snowboard', 'sports ball', 'kite', 'baseball bat', 'baseball glove', 'skateboard', 'surfboard',
    'tennis racket', 'bottle', 'wine glass', 'cup', 'fork', 'knife', 'spoon', 'bowl', 'banana', 'apple',
    'sandwich', 'orange', 'broccoli', 'carrot', 'hot dog', 'pizza', 'donut', 'cake', 'chair', 'couch',
    'potted plant', 'bed', 'dining table', 'toilet', 'tv', 'laptop', 'mouse', 'remote', 'keyboard', 'cell phone',
    'microwave', 'oven', 'toaster', 'sink', 'refrigerator', 'book', 'clock', 'vase', 'scissors', 'teddy bear',
    'hair drier', 'toothbrush'
]
```



### ❤️ Custom TensorFlow.js version

📌 Modify the [index.html](./yolov5_rt_tfjs_src/index.html) file to customize the version of TensorFlow.js

```html
<script src="https://cdn.jsdelivr.net/npm/@tensorflow/tfjs@3.15.0/dist/tf.min.js"></script>
```

❗ Note: **TensorFlow.js version>=3.9.0**, the current version used in this project is **3.15.0**


<h2 align="center">💡 Project Structure</h2>

```shell
.
├──yolov5_rt_tfjs							# project name
│   ├── yolov5								# YOLOv5 project
│   │   ├── yolov5s.pt						# yolov5s model file
│   │   ├── yolov5s_web_model				# pt2tfjs model 01 (used)
│   │   ├── yolov5s_saved_model				# pt2tfjs model 02
│   │   ├── yolov5s.pb						# pt2tfjs model 03
│   │   └── ...								# other
│   ├── yolov5_rt_tfjs_src					# Project main directory
│   │   ├── yolov5s_web_model				# pt2tfjs model 01
│   │   ├── static							# static file directory
│   │   │   └── js							# JavaScript directory
│   │   │  		└── yolov5_rt_tfjs.js		# JS main run file
│   │   │   └── css							# JavaScript directory
│   │   │  		└── main.css				# css style file
│   │   └── index.html						# Project main run file
│   ├── yolov5_rt_tfjs_flask				# Flask version main directory
│   │   ├── templates						# flask template directory
│   │   │  	└── index.html					# flask html file
│   │   ├── server.py						# flask server file
│   │   └── README.md						# Flask version project description
│   ├── yolov5_rt_tfjs_fastapi				# FastAPI version main directory
│   │   ├── templates						# fastapi template directory
│   │   │  	└── index.html					# fastapi html file
│   │   ├── server.py						# fastapi server file
│   │   └── README.md						# Fastapi version project description
│   ├── v_change							# Version history
│   │   ├── v01_change.md					# v0.1 version changes
│   │   ├── v02_change.md					# v0.2 version changes
│   │   └── README.md						# Historical release notes
│   ├── setup.sh							# install script
│   ├── setup_flask.sh						# install script (flask version)
│   ├── setup_fastapi.sh					# install script（fastapi version）
│   ├── LICENSE								# Project License
│   ├── CodeCheck.md						# code inspection
│   ├── .gitignore							# git ignore files
│   ├── .gitmodules							# git submodule file
│   ├── README.md							# project instruction
│   └── README.en.md						# Project Description (English)
```

❗ Note: Some model files need to be generated after installation

<h2 align="center">❤️ Version Creativity</h2>

🔥 yolov5-rt-tfjs v0.2 adopts **bounding box and label decoupling management mode**

<p align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/yolov5_rt_tfjs_v02_change.png">
</p>


<h2 align="center">🔥Installation Tutorial</h2>

### ✅ Method 1: Linux Shell Quick Installation

📌 Create a conda environment

```shell
conda create -n yolo python==3.8
```

📌 Execute [bash script](./setup.sh)

```shell
bash setup.sh
```



### ✅ Method 2: Manual installation

📌 Create a conda environment

```shell
conda create -n yolo python==3.8
conda activate yolo # 进入环境
```

📌 Install dependencies

```shell
pip install -r ./requirements.txt -U
pip install tensorflowjs # >=3.9.0
```

❗ Notice: [tensorflowjs>=3.9.0](https://pypi.org/project/tensorflowjs/)

📌 Clone the project (recursive clone)

```shell
git clone https://gitee.com/CV_Lab/yolov5_rt_tfjs.git --recursive
```

📌 Add the weight file of yolov5 or **custom weight file** (.pt) to the root directory of yolov5

📌 Convert, take [yolov5s.pt](https://github.com/ultralytics/yolov5/releases/download/v6.1/yolov5s.pt) as an example

```shell
python export.py --weights yolov5s.pt --include tfjs
```

📌 Add `yolov5s_web_model` directory to `yolov5_rt_tfjs_src`

<div align="center">
    <a href="./yolov5_rt_tfjs_flask">
        <img src="https://pycver.gitee.io/ows-pics/imgs/flask_logo.png" width="18%"/>
    </a>
    <a href="./yolov5_rt_tfjs_fastapi">
        <img src="https://pycver.gitee.io/ows-pics/imgs/fastapi_logo.png" width="30%"/>
    </a>
</div>
❤️ This project provides 2 extension demos: Flask version and FastAPI version, which aims to show that the project has good portability. Click on the above logo to learn about the detailed installation and usage tutorials.

❗ Note: Before using the following methods, make sure that the `yolov5s_web_model` directory is added to `yolov5_rt_tfjs_src/static`.

### ✅ Method 3: [Flask version](./yolov5_rt_tfjs_flask)

📌 Create a conda environment

```shell
conda create -n yolo python==3.8
conda activate yolo # into the environment
```

📌 Execute the [setup_flask.sh](./setup_flask.sh) script

```shell
bash setup_flask.sh
```



### ✅ Method 4: [FastAPI version](./yolov5_rt_tfjs_fastapi)

📌 Create a conda environment

```shell
conda create -n yolo python==3.8
conda activate yolo # into the environment
```

📌 Execute the [setup_fastapi.sh](./setup_fastapi.sh) script

```shell
bash setup_fastapi.sh
```




<h2 align="center">⚡Usage</h2>

### 💡 Method 1: [VSCode](https://code.visualstudio.com/) run

📌 Install [Live Server](https://github.com/ritwickdey/vscode-live-server-plus-plus) plugin

📌 Run the program with `yolov5_rt_tfjs_src` as the working directory

```shell
cd yolov5_rt_tfjs_src # enter the working directory
code . # open vscode
# Right click, select Open with Live Server, run the program
```




### 💡 Method 2: [Flask](https://github.com/pallets/flask) version

📌 Run the file [server.py](./yolov5_rt_tfjs_flask/server.py)

```shell
python server.py
```

📌 Browser input `http://127.0.0.1:5000/`

<p align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/flask_flow_graph.png">
</p>


### 💡 Method 3: [FastAPI](https://github.com/tiangolo/fastapi) version

📌 Run the file [server.py](./yolov5_rt_tfjs_fastapi/server.py)

```shell
python server.py
```

📌 Browser type `http://localhost:8000/`

<p align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/fastapi_flow_graph.png">
</p>

## 🍷Thanks

- [YOLOv5](https://github.com/ultralytics/yolov5)Founder: [Glenn Jocher](https://github.com/glenn-jocher)
- [Add TensorFlow and TFLite export](https://github.com/ultralytics/yolov5/pull/1127) PR contributor: [Jiacong Fang](https://github.com/zldrobit)
- [TensorFlow.js—Real-Time Object Detection Demo](https://codesandbox.io/s/z364noozrm) author: [Nick Bourdakos](https://github.com/bourdakos1)

