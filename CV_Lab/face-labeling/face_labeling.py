# Face Labeling v0.1
# 创建人：曾逸夫
# 创建时间：2022-03-13


import argparse
import gc
import os
import sys
import time
from collections import Counter
from datetime import datetime
from pathlib import Path

import cv2
import torch

from util.args_yaml import argsYaml
from util.coco_json import coco_json_main
from util.log import log_management
from util.model_opt import yolov5_model_load
from util.obj_opt import get_obj_size
from util.path_opt import increment_path
from util.time_format import time_format
from util.voc_xml import create_xml
from util.yolo_txt import create_yolo_txt

# ---------------------图片和视频输入格式---------------------
IMG_FORMATS = ["jpg", "jpeg", "png", "bmp", "tif", "webp"]
VIDEO_FORMATS = [
    "mp4",
    "avi",
    "wmv",
    "mkv",
    "mov",
    "gif",
    "vob",
    "swf",
    "mpg",
    "flv",
    "3gp",
    "3g2",
]


ROOT_PATH = sys.path[0]  # 根目录
FACELABELING_VERISON = "Face Labeling v0.1"

coco_imgs_list = []  # 图片列表（COCO）
coco_anno_list = []  # 标注列表（COCO）
categories_id = 0  # 类别ID（COCO）


def parse_args(known=False):
    parser = argparse.ArgumentParser(description="Face Labeling v0.1")
    parser.add_argument(
        "--mode", "-m", default="webcam", type=str, help="face labeling mode"
    )
    parser.add_argument(
        "--img_dir", "-imd", default="./data/imgs", type=str, help="image dir"
    )
    parser.add_argument(
        "--video_dir", "-vd", default="./video", type=str, help="video dir"
    )
    parser.add_argument(
        "--model_name", "-mn", default="widerface-m", type=str, help="model name"
    )
    parser.add_argument(
        "--imgName", "-in", default="face_test", type=str, help="image name"
    )
    parser.add_argument(
        "--frame_saveDir", "-fsd", default="FaceFrame", type=str, help="frame save dir"
    )
    parser.add_argument(
        "--frame_dirName", "-fdn", default="frame", type=str, help="frame dir name"
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
        "--max_detNum", "-mdn", default=1000, type=int, help="model max detect obj num"
    )
    parser.add_argument(
        "--inference_size", "-isz", default=640, type=int, help="model inference size"
    )
    parser.add_argument(
        "--cls_name", "-cls", default="face", type=str, help="class name"
    )
    parser.add_argument(
        "--label_dnt_show",
        "-lds",
        action="store_true",
        default=False,
        help="label show",
    )

    args = parser.parse_known_args()[0] if known else parser.parse_args()
    return args


# 人脸检测与信息提取
def face_detect(
    mode,
    frame,
    model,
    frame_id,
    face_id,
    cls_names,
    xyxy_list,
    obj_size_style_list,
    frame_savePath,
    imgName,
    inference_size=640,
    label_dnt_show=False,
    video_name="vide_name.mp4",
):

    global coco_imgs_list
    global coco_anno_list
    global categories_id

    wait_key = cv2.waitKey(20) & 0xFF  # 键盘监听
    xyxy_list = []  # xyxy 列表置空
    obj_size_style_list = []  # 目标尺寸类型
    clsName_list = []  # 类别列表

    img_size = frame.shape  # 帧尺寸
    frame_cp = frame.copy()  # 原始图片副本
    results = model(frame[..., ::-1], size=inference_size)  # 检测结果

    # 判断tensor是否为空
    is_results_null = results.xyxyn[0].shape == torch.Size([0, 6])
    # 显示帧ID
    cv2.putText(
        frame,
        f"Frame ID: {frame_id}",
        (0, 40),
        cv2.FONT_HERSHEY_COMPLEX,
        0.6,
        (0, 255, 0),
        1,
    )

    if not is_results_null:
        for result in results.xyxyn:
            for i in range(len(result)):
                id = int(i)  # 实例ID
                obj_cls_index = int(result[i][5])  # 类别索引
                obj_cls = cls_names[obj_cls_index]  # 类别
                clsName_list.append([obj_cls_index, obj_cls])

                # ------------边框坐标------------
                x0 = float(result[i][:4].tolist()[0])
                y0 = float(result[i][:4].tolist()[1])
                x1 = float(result[i][:4].tolist()[2])
                y1 = float(result[i][:4].tolist()[3])

                # ------------边框实际坐标------------
                x0 = int(img_size[1] * x0)
                y0 = int(img_size[0] * y0)
                x1 = int(img_size[1] * x1)
                y1 = int(img_size[0] * y1)

                xyxy_list.append([x0, y0, x1, y1])  # 边框坐标列表

                obj_size_style_list.append(get_obj_size([x0, y0, x1, y1]))

                conf = float(result[i][4])  # 置信度
                fps = f"{(1000 / float(results.t[1])):.2f}"  # FPS

                if not label_dnt_show:
                    # 标签背景尺寸
                    labelbg_size = cv2.getTextSize(
                        f"{id}-{obj_cls}:{conf:.2f}", cv2.FONT_HERSHEY_COMPLEX, 0.6, 1
                    )
                    # 标签背景
                    cv2.rectangle(
                        frame,
                        (x0, y0),
                        (x0 + labelbg_size[0][0], y0 + labelbg_size[0][1]),
                        (0, 255, 0),
                        thickness=-1,
                    )

                    # 标签
                    cv2.putText(
                        frame,
                        f"{id}-{obj_cls}:{conf:.2f}",
                        (x0, y0 + labelbg_size[0][1]),
                        cv2.FONT_HERSHEY_COMPLEX,
                        0.6,
                        (0, 0, 0),
                        1,
                    )
                # FPS
                cv2.putText(
                    frame,
                    f"FPS: {fps}",
                    (0, 20),
                    cv2.FONT_HERSHEY_COMPLEX,
                    0.6,
                    (0, 255, 0),
                    1,
                )
                # 检测框
                cv2.rectangle(frame, (x0, y0), (x1, y1), (0, 255, 0), 2)

                # 变量回收
                del id, obj_cls_index, obj_cls, x0, y0, x1, y1, conf, fps

        # 人脸数量
        cv2.putText(
            frame,
            f"Face Num: {len(xyxy_list)}",
            (0, 60),
            cv2.FONT_HERSHEY_COMPLEX,
            0.6,
            (0, 255, 0),
            1,
        )

        # ---------------------目标尺寸类型---------------------
        small_num = Counter(obj_size_style_list)["small"]  # 小目标
        medium_num = Counter(obj_size_style_list)["medium"]  # 中目标
        large_num = Counter(obj_size_style_list)["large"]  # 大目标

        cv2.putText(
            frame,
            f"small: {small_num}",
            (0, 80),
            cv2.FONT_HERSHEY_COMPLEX,
            0.6,
            (0, 255, 0),
            1,
        )
        cv2.putText(
            frame,
            f"medium: {medium_num}",
            (0, 100),
            cv2.FONT_HERSHEY_COMPLEX,
            0.6,
            (0, 255, 0),
            1,
        )
        cv2.putText(
            frame,
            f"large: {large_num}",
            (0, 120),
            cv2.FONT_HERSHEY_COMPLEX,
            0.6,
            (0, 255, 0),
            1,
        )

        imgName_faceid = f"{imgName}-{face_id}"  # 图片名称-FaceID

        if mode == "webcam" and wait_key == ord("a"):
            # 捕获视频帧
            cv2.imwrite(
                f"{frame_savePath}/raw/{imgName_faceid}.jpg", frame_cp
            )  # 保存原始图片
            cv2.imwrite(f"{frame_savePath}/tag/{imgName_faceid}.jpg", frame)  # 保存标记图片

            # 创建VOC XML文件
            create_xml(
                f"{imgName_faceid}.jpg",
                f"{frame_savePath}/voc_xml/{imgName_faceid}.jpg",
                img_size,
                clsName_list,
                xyxy_list,
                obj_size_style_list,
                f"{frame_savePath}/voc_xml/{imgName_faceid}.xml",
            )

            create_yolo_txt(
                clsName_list,
                img_size,
                xyxy_list,
                f"{frame_savePath}/yolo_txt/{imgName_faceid}.txt",
            )

            # ------------加入coco图片信息和标注信息------------
            coco_imgs_list.append(
                [
                    face_id,
                    f"{imgName_faceid}.jpg",
                    img_size[1],
                    img_size[0],
                    f"{datetime.now():%Y-%m-%d %H:%M:%S}",
                ]
            )
            coco_anno_list.append(
                [
                    [categories_id + i for i in range(len(xyxy_list))],
                    face_id,
                    clsName_list,
                    xyxy_list,
                ]
            )
            categories_id += len(xyxy_list)

            face_id += 1  # 人脸ID自增

        elif mode == "img":
            # 捕获视频帧
            cv2.imwrite(
                f"{frame_savePath}/raw/{imgName_faceid}.jpg", frame_cp
            )  # 保存原始图片
            cv2.imwrite(f"{frame_savePath}/tag/{imgName_faceid}.jpg", frame)  # 保存标记图片

            # 创建VOC XML文件
            create_xml(
                f"{imgName_faceid}.jpg",
                f"{frame_savePath}/voc_xml/{imgName_faceid}.jpg",
                img_size,
                clsName_list,
                xyxy_list,
                obj_size_style_list,
                f"{frame_savePath}/voc_xml/{imgName_faceid}.xml",
            )

            create_yolo_txt(
                clsName_list,
                img_size,
                xyxy_list,
                f"{frame_savePath}/yolo_txt/{imgName_faceid}.txt",
            )

            # ------------加入coco图片信息和标注信息------------
            coco_imgs_list.append(
                [
                    face_id,
                    f"{imgName_faceid}.jpg",
                    img_size[1],
                    img_size[0],
                    f"{datetime.now():%Y-%m-%d %H:%M:%S}",
                ]
            )
            coco_anno_list.append(
                [
                    [categories_id + i for i in range(len(xyxy_list))],
                    face_id,
                    clsName_list,
                    xyxy_list,
                ]
            )
            categories_id += len(xyxy_list)

            face_id += 1  # 人脸ID自增

        elif mode == "video":
            # 捕获视频帧
            cv2.imwrite(
                f"{frame_savePath}/{video_name}/raw/{imgName_faceid}.jpg", frame_cp
            )  # 保存原始图片
            cv2.imwrite(
                f"{frame_savePath}/{video_name}/tag/{imgName_faceid}.jpg", frame
            )  # 保存标记图片

            # 创建VOC XML文件
            create_xml(
                f"{imgName_faceid}.jpg",
                f"{frame_savePath}/{video_name}/voc_xml/{imgName_faceid}.jpg",
                img_size,
                clsName_list,
                xyxy_list,
                obj_size_style_list,
                f"{frame_savePath}/{video_name}/voc_xml/{imgName_faceid}.xml",
            )

            create_yolo_txt(
                clsName_list,
                img_size,
                xyxy_list,
                f"{frame_savePath}/{video_name}/yolo_txt/{imgName_faceid}.txt",
            )

            # ------------加入coco图片信息和标注信息------------
            coco_imgs_list.append(
                [
                    face_id,
                    f"{imgName_faceid}.jpg",
                    img_size[1],
                    img_size[0],
                    f"{datetime.now():%Y-%m-%d %H:%M:%S}",
                ]
            )
            coco_anno_list.append(
                [
                    [categories_id + i for i in range(len(xyxy_list))],
                    face_id,
                    clsName_list,
                    xyxy_list,
                ]
            )
            categories_id += len(xyxy_list)

            face_id += 1  # 人脸ID自增

    if mode == "webcam":
        return frame, img_size, wait_key, face_id
    elif mode == "img":
        return frame, img_size, face_id
    elif mode == "video":
        return frame, img_size, face_id


# 智能人脸标注
def face_label(
    mode="webcam",
    img_dir="./data/imgs",
    video_dir="./data/videos",
    model_name="widerface-m",
    imgName="face_test",
    frame_saveDir="FaceFrame",
    frame_dirName="frame",
    nms_conf=0.25,
    nms_iou=0.45,
    max_detNum=1000,
    inference_size=640,
    label_dnt_show=False,
    cls_name="face",
):

    model, cls_names = yolov5_model_load(
        ROOT_PATH, model_name, nms_conf, nms_iou, max_detNum, cls_name
    )

    # ----------创建帧文件----------
    frame_savePath = increment_path(
        Path(f"{ROOT_PATH}/{frame_saveDir}") / frame_dirName, exist_ok=False
    )  # 增量运行
    frame_savePath.mkdir(parents=True, exist_ok=True)  # 创建目录
    if mode in ["webcam", "img"]:
        # 创建原始图片目录
        Path(f"{frame_savePath}/raw").mkdir(parents=True, exist_ok=True)
        # 创建标记图片目录
        Path(f"{frame_savePath}/tag").mkdir(parents=True, exist_ok=True)
        # 创建PASCAL VOC XML目录
        Path(f"{frame_savePath}/voc_xml").mkdir(parents=True, exist_ok=True)
        # 创建MS COCO JSON目录
        Path(f"{frame_savePath}/coco_json").mkdir(parents=True, exist_ok=True)
        # 创建YOLO TXT目录
        Path(f"{frame_savePath}/yolo_txt").mkdir(parents=True, exist_ok=True)

    face_id = 0  # 人脸ID
    frame_id = 0  # 帧ID
    xyxy_list = []  # xyxy列表
    obj_size_style_list = []  # 目标尺寸列表

    logTime = f"{datetime.now():%Y-%m-%d %H:%M:%S}"  # 日志时间
    log_management(f"{logTime}\n")  # 记录日志时间

    s_time = time.time()  # 起始时间
    print(f"-------------🔥 {FACELABELING_VERISON} 程序开始！-------------")

    if mode == "webcam":
        cap = cv2.VideoCapture(0)  # 连接设备
        is_capOpened = cap.isOpened()  # 判断设备是否开启

        # 调用face webcam
        if is_capOpened:
            print(f"🚀 欢迎使用{FACELABELING_VERISON}，摄像头连接成功！\n")  # 摄像头连接成功提示

            while is_capOpened:
                _, frame = cap.read()  # 帧读取
                # 人脸检测与信息提取
                frame, img_size, wait_key, face_id = face_detect(
                    mode,
                    frame,
                    model,
                    frame_id,
                    face_id,
                    cls_names,
                    xyxy_list,
                    obj_size_style_list,
                    frame_savePath,
                    imgName,
                    inference_size,
                    label_dnt_show,
                )

                cv2.imshow("capture", frame)  # 显示
                if wait_key == ord("q"):
                    # 退出窗体
                    break

                frame_id += 1  # 帧ID自增

                # 变量回收
                del frame, img_size
                gc.collect()

            coco_json_main(
                cls_names,
                coco_imgs_list,
                coco_anno_list,
                f"{frame_savePath}/coco_json/face_coco.json",
            )

        else:
            print("摄像头连接异常！")

    elif mode == "img":
        # 筛选图片文件
        imgName_list = [
            i for i in os.listdir(img_dir) if i.split(".")[-1].lower() in IMG_FORMATS
        ]
        # 调用 face images
        for i in imgName_list:
            frame = cv2.imread(f"{img_dir}/{i}")

            # 人脸检测与信息提取
            frame, img_size, face_id = face_detect(
                mode,
                frame,
                model,
                frame_id,
                face_id,
                cls_names,
                xyxy_list,
                obj_size_style_list,
                frame_savePath,
                imgName,
                inference_size,
                label_dnt_show,
            )

            print(
                f"帧ID：{frame_id}|({frame_id+1},{len(imgName_list)})|{round((frame_id+1)/len(imgName_list)*100, 2)}%",
                end="\r",
            )

            frame_id += 1  # 帧ID自增

            # 变量回收
            del frame, img_size
            gc.collect()

        print()

        coco_json_main(
            cls_names,
            coco_imgs_list,
            coco_anno_list,
            f"{frame_savePath}/coco_json/face_coco.json",
        )

    elif mode == "video":
        # 筛选图片文件
        videoName_list = [
            i
            for i in os.listdir(video_dir)
            if i.split(".")[-1].lower() in VIDEO_FORMATS
        ]

        for i in videoName_list:
            input_video = cv2.VideoCapture(f"{video_dir}/{i}")
            is_capOpened = input_video.isOpened()  # 判断设备是否开启

            frame_width = input_video.get(3)  # 帧宽度
            frame_height = input_video.get(4)  # 帧高度
            fps = input_video.get(5)  # 帧率
            video_frames = int(input_video.get(7))  # 总帧数

            video_name = i.replace(".", "_")  # 点号取代下划线

            print(
                f"{video_name}，帧宽度：{frame_width}，帧高度：{frame_height}，帧率：{fps}，总帧数：{video_frames}"
            )

            frame_id = 0  # 帧ID

            if is_capOpened:
                # 创建原始图片目录
                Path(f"{frame_savePath}/{video_name}/raw").mkdir(
                    parents=True, exist_ok=True
                )
                # 创建标记图片目录
                Path(f"{frame_savePath}/{video_name}/tag").mkdir(
                    parents=True, exist_ok=True
                )
                # 创建PASCAL VOC XML目录
                Path(f"{frame_savePath}/{video_name}/voc_xml").mkdir(
                    parents=True, exist_ok=True
                )
                # 创建MS COCO JSON目录
                Path(f"{frame_savePath}/{video_name}/coco_json").mkdir(
                    parents=True, exist_ok=True
                )
                # 创建YOLO TXT目录
                Path(f"{frame_savePath}/{video_name}/yolo_txt").mkdir(
                    parents=True, exist_ok=True
                )

                while is_capOpened:
                    wait_key = cv2.waitKey(20) & 0xFF  # 键盘监听
                    ret, frame = input_video.read()
                    if not ret:
                        # 判断空帧
                        break

                    print(
                        f"帧ID：{frame_id}|({frame_id+1},{video_frames})|{round(((frame_id+1)/video_frames)*100, 2)}%",
                        end="\r",
                    )

                    # 人脸检测与信息提取
                    frame, img_size, face_id = face_detect(
                        mode,
                        frame,
                        model,
                        frame_id,
                        face_id,
                        cls_names,
                        xyxy_list,
                        obj_size_style_list,
                        frame_savePath,
                        imgName,
                        inference_size,
                        label_dnt_show,
                        video_name,
                    )

                    # cv2.imshow("video", frame)  # 显示
                    # if wait_key == ord('q'):
                    #     # 中断视频帧保存
                    #     break

                    frame_id += 1

                    del frame, img_size
                    gc.collect

                print()

                coco_json_main(
                    cls_names,
                    coco_imgs_list,
                    coco_anno_list,
                    f"{frame_savePath}/{video_name}/coco_json/face_coco.json",
                )

                input_video.release()
                cv2.destroyAllWindows()

                frame_save_msg = (
                    f"共计{face_id}张人脸图片，保存至{frame_savePath}/{video_name}/raw"
                )
                frametag_save_msg = (
                    f"共计{face_id}张人脸标记图片，保存至{frame_savePath}/{video_name}/tag"
                )
                xml_save_msg = (
                    f"共计{face_id}个人脸xml文件，保存至{frame_savePath}/{video_name}/voc_xml"
                )
                json_save_msg = (
                    f"共计1个人脸json文件，保存至{frame_savePath}/{video_name}/coco_json"
                )
                txt_save_msg = (
                    f"共计{face_id}个人脸txt文件，保存至{frame_savePath}/{video_name}/yolo_txt"
                )
                log_management(
                    f"{frame_save_msg}\n{frametag_save_msg}\n{xml_save_msg}\n{json_save_msg}\n{txt_save_msg}\n"
                )
                print(
                    f"{frame_save_msg}\n{frametag_save_msg}\n{xml_save_msg}\n{json_save_msg}\n{txt_save_msg}\n"
                )

                face_id = 0

            else:
                print("连接视频失败，程序退出！")
                sys.exit()

    else:
        print(f"模式错误，程序退出！")
        sys.exit()

    # ------------------程序结束------------------
    print(f"-------------🔥 {FACELABELING_VERISON} 程序结束！-------------")
    e_time = time.time()  # 终止时间
    total_time = e_time - s_time  # 程序用时
    # 格式化时间格式，便于观察
    outTimeMsg = f"用时：{time_format(total_time)}"
    print(outTimeMsg)  # 打印用时
    log_management(f"{outTimeMsg}\n")  # 记录用时

    if mode in ["webcam", "img"]:
        frame_save_msg = f"共计{face_id}张人脸图片，保存至{frame_savePath}/raw"
        frametag_save_msg = f"共计{face_id}张人脸标记图片，保存至{frame_savePath}/tag"
        xml_save_msg = f"共计{face_id}个人脸xml文件，保存至{frame_savePath}/voc_xml"
        json_save_msg = f"共计1个人脸json文件，保存至{frame_savePath}/coco_json"
        txt_save_msg = f"共计{face_id}个人脸txt文件，保存至{frame_savePath}/yolo_txt"

        log_management(
            f"{frame_save_msg}\n{frametag_save_msg}\n{xml_save_msg}\n{json_save_msg}\n{txt_save_msg}\n"
        )
        print(
            f"{frame_save_msg}\n{frametag_save_msg}\n{xml_save_msg}\n{json_save_msg}\n{txt_save_msg}\n"
        )


def main(args):
    mode = args.mode
    img_dir = args.img_dir
    video_dir = args.video_dir
    model_name = args.model_name
    imgName = args.imgName
    frame_saveDir = args.frame_saveDir
    frame_dirName = args.frame_dirName
    nms_conf = args.nms_conf
    nms_iou = args.nms_iou
    max_detNum = args.max_detNum
    inference_size = args.inference_size
    cls_name = args.cls_name
    label_dnt_show = args.label_dnt_show

    argsYaml(args)  # 脚本参数

    face_label(
        mode,
        img_dir,
        video_dir,
        model_name,
        imgName,
        frame_saveDir,
        frame_dirName,
        nms_conf,
        nms_iou,
        max_detNum,
        inference_size,
        label_dnt_show,
        cls_name,
    )


if __name__ == "__main__":
    args = parse_args()
    main(args)
