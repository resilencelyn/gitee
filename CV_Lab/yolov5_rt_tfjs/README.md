中文简体|[English](./README.en.md)

<p align="center">
<a href="https://gitee.com/CV_Lab/yolov5_rt_tfjs">
<img src="https://pycver.gitee.io/ows-pics/imgs/yolov5_rt_tfjs_logo.png" alt="Simple Icons" >
</a>
<p align="center">
    基于TensorFlow.js的YOLOv5实时目标检测项目，支持自定义模型
</p>
<p align="center">
    轻量安装、使用方便、可移植性强
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







<h2 align="center">🚀更新走势</h2>

- `2022-03-02` **⚡ [yolov5-rt-tfjs v0.3 发行版](https://gitee.com/CV_Lab/yolov5_rt_tfjs/releases/v0.3)正式上线**
- **⚡ [yolov5-rt-tfjs 历史版本](./v_change)**




<h2 align="center">💎项目用途</h2>

### 🔥 自定义目标检测任务

<p align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/yolov5_rt_tfjs.gif">
</p>



### ❤️ 自定义模型

📌 修改[yolov5_rt_tfjs.js](./yolov5_rt_tfjs_src/static/js/yolov5_rt_tfjs.js)文件，自定义**权重文件**和**类别名称**

```javascript
// 权重文件
const weights = '../yolov5s_web_model/model.json';

// 类别名称（中文版）
const cls_names = ['人', '自行车', '汽车', '摩托车', '飞机', '公交车', '火车', '卡车', '船', '红绿灯', '消防栓', '停止标志',
    '停车收费表', '长凳', '鸟', '猫', '狗', '马', '羊', '牛', '象', '熊', '斑马', '长颈鹿', '背包', '雨伞', '手提包', '领带',
    '手提箱', '飞盘', '滑雪板', '单板滑雪', '运动球', '风筝', '棒球棒', '棒球手套', '滑板', '冲浪板', '网球拍', '瓶子', '红酒杯',
    '杯子', '叉子', '刀', '勺', '碗', '香蕉', '苹果', '三明治', '橙子', '西兰花', '胡萝卜', '热狗', '比萨', '甜甜圈', '蛋糕',
    '椅子', '长椅', '盆栽', '床', '餐桌', '马桶', '电视', '笔记本电脑', '鼠标', '遥控器', '键盘', '手机', '微波炉', '烤箱',
    '烤面包机', '洗碗槽', '冰箱', '书', '时钟', '花瓶', '剪刀', '泰迪熊', '吹风机', '牙刷'
]
```



### ❤️ 自定义TensorFlow.js版本

📌 修改[index.html](./yolov5_rt_tfjs_src/index.html)文件，自定义TensorFlow.js的版本

```html
<script src="https://cdn.jsdelivr.net/npm/@tensorflow/tfjs@3.13.0/dist/tf.min.js"></script>
```

❗ 注意：**TensorFlow.js的版本>=3.9.0**，本项目目前使用的版本为**3.15.0**



<h2 align="center">💡项目结构</h2>

```shell
.
├──yolov5_rt_tfjs							# 项目名称
│   ├── yolov5								# YOLOv5项目
│   │   ├── yolov5s.pt						# yolov5s模型文件
│   │   ├── yolov5s_web_model				# pt2tfjs模型01（使用）
│   │   ├── yolov5s_saved_model				# pt2tfjs模型02
│   │   ├── yolov5s.pb						# pt2tfjs模型03
│   │   └── ...								# 其他
│   ├── yolov5_rt_tfjs_src					# 项目主目录
│   │   ├── static							# 静态文件目录
│   │   ├── ├── yolov5s_web_model			# pt2tfjs模型01
│   │   │   ├── js							# JavaScript目录
│   │   │  	│	└── yolov5_rt_tfjs.js		# JS主运行文件
│   │   │   └── css							# JavaScript目录
│   │   │  		└── main.css				# css样式文件
│   │   └── index.html						# 项目主运行文件
│   ├── yolov5_rt_tfjs_flask				# flask版主目录
│   │   ├── templates						# flask模板目录
│   │   │  	└── index.html					# flask html文件
│   │   ├── server.py						# flask server文件
│   │   └── README.md						# flask版项目说明
│   ├── yolov5_rt_tfjs_fastapi				# fastapi版主目录
│   │   ├── templates						# fastapi模板目录
│   │   │  	└── index.html					# fastapi html文件
│   │   ├── server.py						# fastapi server文件
│   │   └── README.md						# fastapi版项目说明
│   ├── yolov5_rt_tfjs_bottle				# bottle版主目录
│   │   ├── views							# bottle模板目录
│   │   │  	└── index.html					# bottle html文件
│   │   ├── server.py						# bottle server文件
│   │   └── README.md						# bottle版项目说明
│   ├── v_change							# 版本历史
│   │   ├── v01_change.md					# v0.1版本变更
│   │   ├── v02_change.md					# v0.2版本变更
│   │   └── README.md						# 历史版本说明
│   ├── setup.sh							# 安装脚本
│   ├── setup_flask.sh						# 安装脚本（flask版）
│   ├── setup_fastapi.sh					# 安装脚本（fastapi版）
│   ├── LICENSE								# 项目许可
│   ├── CodeCheck.md						# 代码检查
│   ├── .gitignore							# git忽略文件
│   ├── .gitmodules							# git子模块文件
│   ├── README.md							# 项目说明
│   └── README.en.md						# 项目说明（英文版）
```

❗ 注：部分模型文件需要安装之后生成

<h2 align="center">❤️ 版本创意</h2>

🔥 yolov5-rt-tfjs v0.2 采用**边界框和标签解耦化管理模式**

<p align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/yolov5_rt_tfjs_v02_change.png">
</p>
<h2 align="center">🔥安装教程</h2>

### ✅ 方法一：Linux Shell 快速安装（Bash 版）

📌 创建conda环境

```shell
conda create -n yolo python==3.8
conda activate yolo # 进入环境
```

📌 执行[bash脚本](./setup.sh)

```shell
bash setup.sh
```



### ✅ 方法二：Linux Shell 手动安装

📌 创建conda环境

```shell
conda create -n yolo python==3.8
conda activate yolo # 进入环境
```

📌 克隆项目（递归式克隆）

```shell
git clone https://gitee.com/CV_Lab/yolov5_rt_tfjs.git --recursive
```

📌 安装依赖

```shell
cd yolov5-tfjs/yolov5
pip install -r ./requirements.txt -U
pip install tensorflowjs # >=3.9.0
```

❗ 注意：[tensorflowjs>=3.9.0](https://pypi.org/project/tensorflowjs/)

📌 将yolov5的权重文件或者**自定义权重文件**（.pt）加入到yolov5的根目录

📌 转换，以[yolov5s.pt](https://github.com/ultralytics/yolov5/releases/download/v6.1/yolov5s.pt) 为例

```shell
python export.py --weights yolov5s.pt --include tfjs
```

📌 将`yolov5s_web_model` 目录加入到`yolov5_rt_tfjs_src/static` 中

<div align="center">
    <a href="./yolov5_rt_tfjs_flask">
        <img src="https://pycver.gitee.io/ows-pics/imgs/flask_logo.png" width="15%"/>
    </a>
    <a href="./yolov5_rt_tfjs_fastapi">
        <img src="https://pycver.gitee.io/ows-pics/imgs/fastapi_logo.png" width="20%"/>
    </a>
    <a href="./yolov5_rt_tfjs_bottle">
        <img src="https://pycver.gitee.io/ows-pics/imgs/bottle_logo.png" width="15%"/>
    </a>
</div>



❤️ 本项目提供了3种扩展演示：Flask版、FastAPI版和Bottle版，旨在表明该项目具有良好的可移植性。点击上面的logo可以了解详细的安装和使用教程。

❗ 注：在使用以下方法前，必须确保`yolov5s_web_model` 目录加入到`yolov5_rt_tfjs_src/static` 中。

### ✅ 方法三：[Flask版](./yolov5_rt_tfjs_flask)

📌 创建conda环境

```shell
conda create -n yolo python==3.8
conda activate yolo # 进入环境
```

📌 执行[setup_flask.sh](./setup_flask.sh)脚本

```shell
bash setup_flask.sh
```



### ✅ 方法四：[FastAPI版](./yolov5_rt_tfjs_fastapi)

📌 创建conda环境

```shell
conda create -n yolo python==3.8
conda activate yolo # 进入环境
```

📌 执行[setup_fastapi.sh](./setup_fastapi.sh)脚本

```shell
bash setup_fastapi.sh
```



### ✅ 方法五：[Bottle版](./yolov5_rt_tfjs_bottle)

📌 创建conda环境

```shell
conda create -n yolo python==3.8
conda activate yolo # 进入环境
```

📌 执行[setup_bottle.sh](./setup_bottle.sh)脚本

```shell
bash setup_bottle.sh
```



<h2 align="center">⚡使用教程</h2>

### 💡 方法一：[VSCode](https://code.visualstudio.com/)版

📌 安装[Live Server](https://github.com/ritwickdey/vscode-live-server-plus-plus) 插件

📌 将`yolov5_rt_tfjs_src`作为工作目录，运行程序

```shell
cd yolov5_rt_tfjs_src # 进入工作目录
code . # 打开vscode
# 右键单击，选择Open with Live Server，运行程序
```




### 💡 方法二：[Flask](https://github.com/pallets/flask)版

📌 运行文件[server.py](./yolov5_rt_tfjs_flask/server.py)

```shell
python server.py
```

📌 浏览器输入`http://127.0.0.1:5000/`

<p align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/flask_flow_graph.png">
</p>


### 💡 方法三：[FastAPI](https://github.com/tiangolo/fastapi)版

📌 运行文件[server.py](./yolov5_rt_tfjs_fastapi/server.py)

```shell
python server.py
```

📌 浏览器输入`http://localhost:8000/`

<p align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/fastapi_flow_graph.png">
</p>


### 💡 方法四：[Bottle](https://github.com/bottlepy/bottle)版

📌 运行文件[server.py](./yolov5_rt_tfjs_bottle/server.py)

```shell
python server.py
```

📌 浏览器输入`http://localhost:8080/`



## 🍷鸣谢

- [YOLOv5](https://github.com/ultralytics/yolov5)创始人：[Glenn Jocher](https://github.com/glenn-jocher)
- [Add TensorFlow and TFLite export](https://github.com/ultralytics/yolov5/pull/1127) PR贡献人：[Jiacong Fang](https://github.com/zldrobit)
- [TensorFlow.js—Real-Time Object Detection Demo](https://codesandbox.io/s/z364noozrm) 作者：[Nick Bourdakos](https://github.com/bourdakos1)

