<p align="center">
    <img src="https://pycver.gitee.io/ows-pics/imgs/yolov5_rt_tfjs_flask_title.png">
</p>

# 💎 YOLOv5 RT TFJS （Flask版）

## 创建人：曾逸夫

<p align="center" >
<img src="https://pycver.gitee.io/ows-pics/imgs/flask_flow_graph.png">
</p>

<h3 align="center">🔥安装教程</h2>


📌 第一步：创建conda环境

```shell
conda create -n yolo python==3.8
conda activate yolo # 进入环境
```

📌 第二步：安装依赖

```shell
pip install flask -U
```

📌 第三步：将`yolov5_rt_tfjs_src/static` 复制到`yolov5_rt_tfjs_flask` 中

📌 第二步和第三步也可以通过执行[setup_flask.sh](https://gitee.com/CV_Lab/yolov5_rt_tfjs/blob/master/setup_flask.sh) 完成



<h3 align="center">⚡使用教程</h2>

📌 运行文件[server.py](https://gitee.com/CV_Lab/yolov5_rt_tfjs/blob/master/yolov5_rt_tfjs_flask/server.py)

```shell
python server.py
```

📌 浏览器输入`http://127.0.0.1:5000/`



### 🍷鸣谢

- [Flask官网](https://github.com/pallets/flask)
