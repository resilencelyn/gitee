# YOLOv5模型管理
# 创建人：曾逸夫
# 创建时间：2022-03-16


import torch


# 模型加载
def yolov5_model_load(file_path, model_name, nms_conf, nms_iou, max_detNum, cls_name):
    try:
        # 本地模型路径
        local_model_path = f"{file_path}/yolov5"

        # 加载模型
        model = torch.hub.load(
            local_model_path,
            "custom",
            path=f"{local_model_path}/{model_name}",
            source="local",
        )

        # ------模型调参------
        model.conf = nms_conf  # NMS 置信度阈值
        model.iou = nms_iou  # NMS IOU阈值
        model.max_det = max_detNum  # 最大检测目标数

        # 类别名称
        cls_names = [cls_name]

        print("模型加载成功！")

        return model, cls_names
    except Exception as e:
        print("模型加载失败！")
        print(e)
        return False
