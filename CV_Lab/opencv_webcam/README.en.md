[中文简体](./README.md)|English
<p align="center">
<a href="https://gitee.com/CV_Lab/opencv_webcam">
<img src="https://pic.imgdb.cn/item/61f116412ab3f51d918b7bb1.png" alt="Simple Icons" >
</a>
<p align="center">
    Webcam program based on OpenCV, suitable for computer vision data acquisition, real-time video frame collection, etc.
</p>
<p align="center">
    It has the characteristics of convenient installation, simple operation and cross-platform.
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


<h2 align="center">🚀Update Trend</h2>

- `2022-02-06` **⚡ [opencv-webcam-script v0.5 release version](https://gitee.com/CV_Lab/opencv_webcam/releases/v0.5)officially launched**
- `2022-02-06` **⚡ [opencv-webcam-script v0.5 docker version](https://hub.docker.com/r/zengdockerdocker/opencv-webcam-script)officially launched**
- `2022-02-06` **⚡ [opencv-webcam-script v0.5.1 PyPI version](https://pypi.org/project/opencv-webcam-script/0.5.1/)officially launched**
- `2022-02-06` **⚡ [opencv-webcam-script v0.5 Jupyter version](https://gitee.com/CV_Lab/opencv_webcam/blob/master/tutorial/ows_jupyter.ipynb)officially launched**
- ⚡ **[opencv-webcam-script historical version](./v_change)**



<h2 align="center">💎Project purpose</h2>

### 🔥 Quickly create object detection datasets
####  📌 Project 1: [Gesture recognition demo based on YOLOv5](yolov5_GestureData.md)

<p align="center" >
<a href="https://gitee.com/CV_Lab/opencv_webcam/blob/master/GestureData_yolov5.md">
<img src="https://pic.imgdb.cn/item/61f9be4e2ab3f51d9184300d.gif"></a>
</p>



<h2 align="center">💡Project Structure</h2>

```
.
├── opencv_webcam							# project name
│   ├── utils								# Toolkit
│   │   ├── args_yaml.py					# Instruction management
│   │   ├── compress.py						# Compression management
│   │   ├── frame_opt.py					# Frame save management
│   │   ├── hotkey.py						# Hotkey management
│   │   ├── fonts_opt.py					# Font management
│   │   ├── plot.py							# Drawing management
│   │   ├── __init__.py						# Toolkit initialization
│   │   ├── log.py							# log management
│   │   ├── ows_path.py						# Save path management
│   │   └── time_format.py					# time format
│   ├── tutorial							# ows tutorial
│   │   ├── ows_pypi.md						# ows PyPI version tutorial
│   │   ├── ows_docker.md					# ows docker version tutorial
│   │   └── ows_jupyter.ipynb				# ows Jupyter version tutorial
│   ├── v_change							# Version history
│   │   ├── v01_change.md					# v0.1 version change
│   │   ├── v02_change.md					# v0.2 version change
│   │   ├── v03_change.md					# v0.3 version change
│   │   ├── v04_change.md					# v0.4 version change
│   │   └── README.md						# Version history description
│   ├── __init__.py							# Project initialization
│   ├── opencv_webcam.py					# Script main run file
│   ├── LICENSE								# Project License
│   ├── CodeCheck.md						# Code Check
│   ├── setup.sh							# Environment installation script
│   ├── fonts.sh							# Font download script
│   ├── Dockerfile							# docker build file
│   ├── .gitignore							# git ignore files
│   ├── .dockerignore						# docker ignore file
│   ├── GestureData_yolov5.md				# Gesture&YOLOv5 demo
│   ├── README.md							# Project description
│   ├── README.en.md						# Project description (English version)
│   └── requirements.txt					# Script dependencies
```



<h2 align="center">🔥Installation Tutorial</h2>

### ✅ Method 1: Linux Shell Installation (Development Edition)

📌 Step 1: Clone the project

```shell
git clone https://gitee.com/CV_Lab/opencv_webcam.git # clone project
```

📌 Step 2: Create a virtual environment

```shell
# Create a conda virtual environment, taking python 3.8 as an example
conda create -n ows python==3.8 # The virtual environment name is ows
conda activate ows # Activate the virtual environment
```

📌 Step 3: Install the script

```shell
pip install -r ./requirements.txt -U # Install OpenCV Webcam Script
```

📌 Steps 2 and 3 can also be performed via [setup.sh](./setup.sh)

```shell
bash ./setup.sh
```

❗Note: **OWS v0.6** and above need to run in the environment of [**Python>=3.8.0**](https://www.python.org/)



### ✅ Method 2: pip quick installation (PyPI version)

#### 🔥 [opencv-webcam-script PyPI version detailed tutorial](./tutorial/ows_pypi.md)

#### 🎨 Easy Tutorial

📌 Step 1: Create an ows virtual environment, see Method 1

📌 Step 2: Execute the pip command

```shell 
pip install opencv-webcam-script==0.5.1
```

📌 Step 3: Write a python program

```python
from opencv_webcam.opencv_webcam import webcam_opencv

# ------------Example of a few functions------------
webcam_opencv() # regular call
webcam_opencv(is_autoSaveFrame=True) # Auto save frames
webcam_opencv(is_autoSaveFrame=True, is_compress=True) # compressed frame
```



### ✅ Method 3: docker image installation (docker version)

#### 🔥 [opencv-webcam-script docker version detailed tutorial](./tutorial/ows_docker.md)

#### 🎨 Easy Tutorial

📌 Step 1: Download the mirror

```shell
sudo docker pull zengdockerdocker/opencv-webcam-script:v0.5 # Mirror pull
```

📌 Step 2: Create the container

```shell
xhost +local:root # Allow root user access to running X server (important)
# Create a container named ows
sudo docker run --name=ows --ipc=host -it -v /tmp/.X11-unix:/tmp/.X11-unix -e DISPLAY=unix$DISPLAY --device=/dev/video0:/dev/video0 zengdockerdocker/opencv-webcam-script:v0.5
```

📌 Step 3: Run the program

```shell
python3 opencv_webcam.py # run script
python3 opencv_webcam.py -isasf # Auto save frames

# Get docker frame data to this machine
sudo docker cp Container name: container directory local host directory
sudo docker cp ows:/usr/src/app/WebcamFrame /home/username # Example
```



### ✅ Method 4: Jupyter installation (Jupyter version)

#### 🔥 [opencv-webcam-script Jupyter version detailed tutorial](./tutorial/ows_jupyter.ipynb)

#### 🎨 Easy Tutorial

📌 Step 1: Installation

```shell
# First create a virtual environment manually
# conda create -n ows python==3.8 # The virtual environment name is ows
# conda activate ows # Activate the virtual environment
!git clone https://gitee.com/CV_Lab/opencv_webcam.git # clone
%cd opencv_webcam
%pip install -qr requirements.txt -U # install
```

📌 Step 2: Run

```shell
!python opencv_webcam.py # Press the q key to exit by default
!python opencv_webcam.py -isasf # Automatically save video frames
```

**Note: [Jupyter version of ows ](https://gitee.com/CV_Lab/opencv_webcam/blob/master/tutorial/ows_jupyter.ipynb)program can run unit instructions through `Ctrl+Enter`**



<h2 align="center">⚡Usage</h2>

####  💡 regular use

```shell
# Press the q key to exit by default
python opencv_webcam.py
```



####  💡 Device Selection

```shell
# Multi-camera switching example, the default is 0
python opencv_webcam.py -dev 0
python opencv_webcam.py -dev 1
python opencv_webcam.py -dev 2

# RTSP
python opencv_webcam.py -dev rtsp://username:password@xxx.xxx.xxx
```



####  💡 set escape key

```shell
# By default press the q key to exit
python opencv_webcam.py -q z # set z key to exit
python opencv_webcam.py -q k # set k key to exit
```



####  💡 Auto save frames

```shell
python opencv_webcam.py -isasf
```



#### 💡 Save frames every n frames

```shell
# save a frame every 10 frames
python opencv_webcam.py -isasf -fns 10
```



#### 💡 Save frames manually

```shell
# By default press a key to capture a frame
python opencv_webcam.py -ishsf
```



#### 💡 custom capture key

```shell
# Set the z key as the capture key, the default is the a key
python opencv_webcam.py -ishsf -fck z
```



#### 💡 Reshape frame size (custom width and height)

```shell
# reshape width 300 height 200
python opencv_webcam.py -isasf -isrf -rf 300,200 # Automatic version
python opencv_webcam.py -ishsf -isrf -rf 300,200 # Manual version
```



#### 💡 Reshape frame size (custom aspect scaling)

```shell
# The aspect ratio is 0.5
python opencv_webcam.py -isasf -isrf -rrf 0.5 # Automatic version
python opencv_webcam.py -ishsf -isrf -rrf 0.5 # Manual version
```



#### 💡 Custom save path

```shell
# Set the save path, the default save path is ./WebcamFrame
python opencv_webcam.py -fsd custom_dir -isasf # Take the automatic version as an example
```



####  💡 custom frame directory name

```shell
# Set the frame directory name, the default save path is frames
python opencv_webcam.py -fdn frames_custom -isasf # Take the automatic version as an example
```




####  💡 Custom frame name prefix

```shell
# Set the prefix name of the frame image
python opencv_webcam.py -isasf -fnp webcam
```



#### 💡 Set the frame save format

```shell
# Set JPG quality to 100, default to 95
python opencv_webcam.py -isasf -fss jpg -jq 100

# Set PNG quality to 5, default is 3
python opencv_webcam.py -isasf -fss png -jq 5
```



#### 💡 set pause button

```shell
# Set the w key as the pause key, the default is the p key
python opencv_webcam.py -p w # Press any key to continue
```



#### 💡 Set the number of frames to save

```shell
# Set to save 100 frames
python opencv_webcam.py -isasf -afn 100
```



#### 💡 log settings

```shell
# The log file is saved in the project root directory by default, ows.log
python opencv_webcam.py

# Set the log file name and type, including .log, .txt, .data
python opencv_webcam.py -ln ows02.txt

# Set the log saving method, the default is append mode
python opencv_webcam.py -lm w # set to overlay mode
```

 

#### 💡 video frame compression

```shell
# Regular compression, default ows.zip (take the automatic version as an example)
python opencv_webcam.py -isasf -isc

# Customize the compressed file name
python opencv_webcam.py -isasf -isc -cn ows02

# Automatically name archives
python opencv_webcam.py -isasf -isc -isacn

# Custom compression mode, default is write overwrite
python opencv_webcam.py -isasf -isc -cs tar -cm w:gz # tar compression
python opencv_webcam.py -isasf -isc -cm a # Append mode (a mode is only for zip) Note: This command is only available in v0.4
```



#### 💡 Date-Frame Number Chart

Every time this function runs the script, the program will automatically generate a `date-frames graph` based on the `date_time_frames.csv` file.

<p align="center" >
<img src="https://pic.imgdb.cn/item/61f520d12ab3f51d915457e4.png">
</p>



#### 💡 Instruction query

```shell
# query script parameters
python opencv_webcam.py --help
```

<details open>
<summary>Command query result</summary>

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
