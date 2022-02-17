<p align="center" >
<a href="https://www.kaggle.com/zenggis/gesture-v1">
<img src="https://pic.imgdb.cn/item/61fdd1912ab3f51d912185c7.png"></a>
</p>


# 💎 基于YOLOv5的手势识别demo

## 创建人：曾逸夫

<p align="left">
<a href="https://www.kaggle.com/zenggis/gesture-v1"><img src="https://img.shields.io/badge/Kaggle-Gesture%20v1.0-informational?logo=kaggle" alt="code check" /></a>
<a href="https://www.gnu.org/licenses/agpl-3.0.en.html"><img src="https://img.shields.io/badge/Gesture%20v1.0%20License-AGPL%203.0-blueviolet" alt="code check" /></a>
</p>



### 🚀 项目简介

**本项目是基于[YOLOv5 v6.0](https://github.com/ultralytics/yolov5)进行的训练，为demo级别。旨在证明[opencv-webcam-script](https://gitee.com/CV_Lab/opencv_webcam)脚本在制作计算机视数据集中的价值和意义。**



### 🎉 demo效果

<p align="center" >
<img src="https://pic.imgdb.cn/item/61f9be4e2ab3f51d9184300d.gif">
</p>


### 🔥 Gesture v1.0 数据集

**Gesture v1.0数据集是基于[opencv-webcam-script](https://gitee.com/CV_Lab/opencv_webcam)采集的，并通过[label-studio PyPi版](https://pypi.org/project/label-studio/)进行标注，标注格式为[Pascal VOC XML](http://host.robots.ox.ac.uk/pascal/VOC/)**


<p align="center" >
<img src="https://pic.imgdb.cn/item/61fa28522ab3f51d91d80bca.png">
</p>

#### ❤️ 数据集下载地址

⚡ `2022-02-05`  **Gesture v1.0 数据集在[Kaggle](https://www.kaggle.com/)正式开源**

📌 原始数据集：[gesture_v1_raw](https://www.kaggle.com/zenggis/gesture-v1?select=gesture_v1_raw)

📌 标注数据集：[gesture_v1_annotation](https://www.kaggle.com/zenggis/gesture-v1?select=gesture_v1_annotation)

📌 yolov5训练&验证数据集：[gesture_v1_yolov5](https://www.kaggle.com/zenggis/gesture-v1?select=gesture_v1_yolov5)



#### ⚡⚡⚡ opencv-webcam-script 脚本使用

Gesture v1.0数据集是使用**opencv-webcam-script v0.5**采集的。在自动版下，每隔5帧采集一次，一共采集500张图片，指令如下：

```shell
python opencv_webcam.py -isasf -fsd gesture -fdn one -fns 5 -afn 2500 # 以类别one为例，其他类别以此类推
```


<p align="center" >
<img src="https://pic.imgdb.cn/item/61fb1c112ab3f51d91b8cc5e.png">
</p>

采集日志如下：

```shell
2022-01-30 21:01:18
自动版：共计500帧，已保存在：gesture/one
用时：2分43.001秒
2022-01-30 21:05:53
自动版：共计500帧，已保存在：gesture/two
用时：2分44.675秒
2022-01-30 21:11:17
自动版：共计500帧，已保存在：gesture/three
用时：2分40.040秒
2022-01-30 21:14:37
自动版：共计500帧，已保存在：gesture/four
用时：2分43.015秒
2022-01-30 21:17:51
自动版：共计500帧，已保存在：gesture/five
用时：2分42.426秒
```



#### 💡 数据集属性


|  数据集属性  | 参数值  |
| :----------: | :-----: |
| 原始总图片数 |  2500   |
| 总标记图片数 |  2417   |
|   总实例数   |  4159   |
|   图片尺寸   | 640X480 |
|    总容量    | 183.2MB |



#### 💡 类别与实例数

| 类别  | 实例数 | 标记图片数 | 原始图片数 |
| :---: | :----: | :--------: | :--------: |
|  one  |  894   |    495     |    500     |
|  two  |  804   |    487     |    500     |
| three |  837   |    484     |    500     |
| four  |  807   |    480     |    500     |
| five  |  817   |    471     |    500     |



### 🗂️ demo数据集配置

#### 📌 图片配置

|  名称  | 数量 | 占比  |
| :----: | :--: | :---: |
| 训练集 | 1916 | 0.793 |
| 验证集 | 501  | 0.207 |

#### 📌 实例配置

| 类别  | 实例总数 | 训练集数 | 验证集数 | 训练集占比 | 验证集占比 |
| :---: | :------: | :------: | :------: | :--------: | :--------: |
|  one  |   894    |   709    |   185    |   79.31    |   20.69    |
|  two  |   804    |   635    |   169    |   78.98    |   21.02    |
| three |   837    |   670    |   167    |   80.05    |   19.95    |
| four  |   807    |   636    |   171    |   78.81    |   21.19    |
| five  |   817    |   655    |   162    |   80.17    |   19.83    |



### 🗂️ 实验配置


#### 📌 软件配置

**本项目基于Ubuntu Linux操作系统，使用[label-studio（PyPi版）](https://pypi.org/project/label-studio/)进行数据标注，同时使用[YOLOv5 v6.0（克隆版）](https://github.com/ultralytics/yolov5)进行训练**

|    软件名称    |         版本型号         |
| :------------: | :----------------------: |
|    操作系统    | Ubuntu Linux 20.04.2 LTS |
|    显卡驱动    |      NVIDIA 460.80       |
|      CUDA      |          11.1.1          |
|     cuDNN      |         8.0.5.39         |
|    Anaconda    |          4.10.1          |
|     Python     |          3.8.10          |
|    PyTorch     |          1.9.0           |
|    ❤️ YOLOv5    |      v6.0（克隆版）      |
| ❤️ label-studio |      1.4（PyPi版）       |



#### 📌 硬件配置

| 硬件名称 |            版本型号             | 容量  | 数量 |
| :------: | :-----------------------------: | :---: | :--: |
|   CPU    |         CORE i9 10900K          |   —   |  1   |
|   GPU    |  ROG-STRIX-RTX3090-O24G-GAMING  |  24G  |  1   |
|   内存   | CORSAIR VENGEANCE DDR4 16G 3600 |  16G  |  2   |
| 固态硬盘 |      Samsung 980 PRO 500G       | 500G  |  2   |
|   电源   |       USCORSAIR RM 1000X        | 1000W |  1   |



### ✨ demo结果

- 本项目基于[yolov5 v6.0](https://github.com/ultralytics/yolov5)进行训练，batch size为`64`，预训练模型为[yolov5s.pt](https://github.com/ultralytics/yolov5/releases/download/v6.0/yolov5s.pt)，其他参数为默认配置。
- 训练指令

```shell
python train.py --data data/gesture-data.yaml --cfg models/yolov5s.yaml --weights weights/yolov5s.pt --batch-size 64 --epochs 200 --name gesture-s
```

- 本结果采用`YOLOv5 v6.0默认评价标准`

| 类别  |  AP   | AP50  |
| :---: | :---: | :---: |
|  all  | 0.816 | 0.991 |
|  one  | 0.763 | 0.991 |
|  two  | 0.785 | 0.986 |
| three | 0.817 | 0.994 |
| four  | 0.855 | 0.989 |
| five  | 0.861 | 0.993 |

- 其他参数

|   模型    |    数据集    | 层数 | #Param  | GFLOPs | 显存占用 | FPS  | 训练时间 |
| :-------: | :----------: | :--: | :-----: | :----: | :------: | :--: | :------: |
| gesture_s | Gesture v1.0 | 213  | 7023610 |  15.8  | 15.4 GB  | 189  | 0.586 h  |

注：上表中`FPS`是通过yolov5 v6.0的`detect.py`运行得到的，运行指令：

```shell
python detect.py --source ../datasets/gesture/images/val/ --weights runs/train/gesture-s/weights/best.pt --name gesture-s
```

[YOLOv5 FPS计算公式](https://github.com/ultralytics/yolov5/pull/4741#issuecomment-916861943)：

```shell
FPS = 1000ms/inference time(ms)
```

**注：demo效果由于是在web框架中检测的，所以其FPS较shell中的低**

<h4 align="center">训练损失过程</h4>


<p align="center" >
<img src="https://pic.imgdb.cn/item/61fb21b22ab3f51d91bcbf40.png">
</p>


### 🍷 鸣谢

- [YOLOv5](https://github.com/ultralytics/yolov5)创始人：[Glenn Jocher](https://github.com/glenn-jocher)

- [label-studio](https://github.com/heartexlabs/label-studio) 标注软件