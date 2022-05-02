# Gradio YOLOv5 Det v0.1
# 创建人：曾逸夫
# 创建时间：2022-04-03

import argparse
import csv
import os
import sys
from pathlib import Path

import gradio as gr
import torch
import yaml
from PIL import Image

ROOT_PATH = sys.path[0]  # 根目录

# 本地模型路径
local_model_path = f"{ROOT_PATH}/yolov5"

# 模型名称临时变量
model_name_tmp = ""

# 设备临时变量
device_tmp = ""

# 文件后缀
suffix_list = [".csv", ".yaml"]


def parse_args(known=False):
    parser = argparse.ArgumentParser(description="Gradio YOLOv5 Det v0.1")
    parser.add_argument(
        "--model_name", "-mn", default="yolov5s", type=str, help="model name"
    )
    parser.add_argument(
        "--model_cfg",
        "-mc",
        default="./model_config/model_name_p5_all.yaml",
        type=str,
        help="model config",
    )
    parser.add_argument(
        "--cls_name",
        "-cls",
        default="./cls_name/cls_name.yaml",
        type=str,
        help="cls name",
    )
    parser.add_argument(
        "--nms_conf",
        "-conf",
        default=0.5,
        type=float,
        help="model NMS confidence threshold",
    )
    parser.add_argument(
        "--nms_iou", "-iou", default=0.45, type=float, help="model NMS IoU threshold"
    )

    parser.add_argument(
        "--label_dnt_show",
        "-lds",
        action="store_true",
        default=False,
        help="label show",
    )
    parser.add_argument(
        "--device", "-dev", default="cpu", type=str, help="cuda or cpu",
    )
    parser.add_argument(
        "--inference_size", "-isz", default=640, type=int, help="model inference size"
    )

    args = parser.parse_known_args()[0] if known else parser.parse_args()
    return args


#  模型加载
def model_loading(model_name, device):

    try:
        # 加载本地模型
        model = torch.hub.load(
            local_model_path,
            "custom",
            path=f"{local_model_path}/{model_name}",
            source="local",
            device=device,
            _verbose=False,
        )
    except Exception as e:
        print("模型加载失败！")
        print(e)
        return False
    else:
        return model


# 检测信息
def export_json(results, model, img_size):

    return [
        [
            {
                "id": i,
                "class": int(result[i][5]),
                "class_name": model.model.names[int(result[i][5])],
                # "class_name": model_cls_name_cp[int(result[i][5])],
                "normalized_box": {
                    "x0": round(result[i][:4].tolist()[0], 6),
                    "y0": round(result[i][:4].tolist()[1], 6),
                    "x1": round(result[i][:4].tolist()[2], 6),
                    "y1": round(result[i][:4].tolist()[3], 6),
                },
                "confidence": round(float(result[i][4]), 2),
                "fps": round(1000 / float(results.t[1]), 2),
                "width": img_size[0],
                "height": img_size[1],
            }
            for i in range(len(result))
        ]
        for result in results.xyxyn
    ]


# YOLOv5图片检测函数
def yolo_det(img, device, model_name, inference_size, conf, iou, label_opt, model_cls):

    global model, model_name_tmp, device_tmp

    if model_name_tmp != model_name:
        # 模型判断，避免反复加载
        model_name_tmp = model_name
        model = model_loading(model_name_tmp, device)
    elif device_tmp != device:
        device_tmp = device
        model = model_loading(model_name_tmp, device)

    # -----------模型调参-----------
    model.conf = conf  # NMS 置信度阈值
    model.iou = iou  # NMS IOU阈值
    model.max_det = 1000  # 最大检测框数
    model.classes = model_cls  # 模型类别

    results = model(img, size=inference_size)  # 检测
    results.render(labels=label_opt)  # 渲染

    det_img = Image.fromarray(results.imgs[0])  # 检测图片

    det_json = export_json(results, model, img.size)[0]  # 检测信息

    return det_img, det_json


# yaml文件解析
def yaml_parse(file_path):
    return yaml.safe_load(open(file_path, encoding="utf-8").read())


# yaml csv 文件解析
def yaml_csv(file_path, file_tag):
    file_suffix = Path(file_path).suffix
    if file_suffix == suffix_list[0]:
        # 模型名称
        file_names = [i[0] for i in list(csv.reader(open(file_path)))]  # csv版
    elif file_suffix == suffix_list[1]:
        # 模型名称
        file_names = yaml_parse(file_path).get(file_tag)  # yaml版
    else:
        print(f"{file_path}格式不正确！程序退出！")
        sys.exit()

    return file_names


def main(args):
    gr.close_all()

    global model

    slider_step = 0.05  # 滑动步长

    nms_conf = args.nms_conf
    nms_iou = args.nms_iou
    label_opt = args.label_dnt_show
    model_name = args.model_name
    model_cfg = args.model_cfg
    cls_name = args.cls_name
    device = args.device
    inference_size = args.inference_size

    # 模型加载
    model = model_loading(model_name, device)

    model_names = yaml_csv(model_cfg, "model_names")
    model_cls_name = yaml_csv(cls_name, "model_cls_name")

    # model_cls_name_cp = model_cls_name.copy()  # 类别名称

    # -------------------输入组件-------------------
    inputs_img = gr.inputs.Image(type="pil", label="原始图片")
    inputs_device = gr.inputs.Dropdown(
        choices=["0", "cpu"], default=device, type="value", label="设备"
    )
    inputs_model = gr.inputs.Dropdown(
        choices=model_names, default=model_name, type="value", label="模型"
    )
    inputs_size = gr.inputs.Radio(
        choices=[320, 640], default=inference_size, label="推理尺寸"
    )
    input_conf = gr.inputs.Slider(
        0, 1, step=slider_step, default=nms_conf, label="置信度阈值"
    )
    inputs_iou = gr.inputs.Slider(
        0, 1, step=slider_step, default=nms_iou, label="IoU 阈值"
    )
    inputs_label = gr.inputs.Checkbox(default=(not label_opt), label="标签显示")
    inputs_clsName = gr.inputs.CheckboxGroup(
        choices=model_cls_name, default=model_cls_name, type="index", label="类别"
    )

    # 输入参数
    inputs = [
        inputs_img,  # 输入图片
        inputs_device,  # 设备
        inputs_model,  # 模型
        inputs_size,  # 推理尺寸
        input_conf,  # 置信度阈值
        inputs_iou,  # IoU阈值
        inputs_label,  # 标签显示
        inputs_clsName,  # 类别
    ]
    # 输出参数
    outputs = gr.outputs.Image(type="pil", label="检测图片")
    outputs02 = gr.outputs.JSON(label="检测信息")

    # 标题
    title = "基于Gradio的YOLOv5通用目标检测系统"
    # 描述
    description = "<div align='center'>可自定义目标检测模型、安装简单、使用方便</div>"

    # 示例图片
    examples = [
        ["./img_example/bus.jpg", "cpu", "yolov5s", 640, 0.6, 0.5, True, ["人", "公交车"],],
        [
            "./img_example/Millenial-at-work.jpg",
            "0",
            "yolov5l",
            320,
            0.5,
            0.45,
            True,
            ["人", "椅子", "杯子", "笔记本电脑"],
        ],
        [
            "./img_example/zidane.jpg",
            "0",
            "yolov5m",
            640,
            0.25,
            0.5,
            False,
            ["人", "领带"],
        ],
    ]

    # 接口
    gr.Interface(
        fn=yolo_det,
        inputs=inputs,
        outputs=[outputs, outputs02],
        title=title,
        description=description,
        examples=examples,
        theme="seafoam",
        # live=True, # 实时变更输出
        flagging_dir="run"  # 输出目录
        # ).launch(inbrowser=True, auth=['admin', 'admin'])
    ).launch(
        inbrowser=True,  # 自动打开默认浏览器
        show_tips=True,  # 自动显示gradio最新功能
        # favicon_path="./icon/logo.ico",
    )


if __name__ == "__main__":
    args = parse_args()
    main(args)