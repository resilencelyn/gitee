# 视频帧操作函数
# 创建人：曾逸夫
# 创建时间：2022-01-04

import cv2
import sys
import numpy as np
from PIL import Image, ImageDraw, ImageFont


ROOT_PATH = sys.path[0]  # 项目根目录


# 帧保存
def frame_opt(frame,
              frame_savePath,
              frame_num,
              is_resizeFrame,
              resize_frame,
              resizeRatio_frame,
              frame_namePrefix,
              frame_saveStyle,
              jpg_quality,
              png_quality):

    # 判断图片质量范围
    if (jpg_quality < 0 or jpg_quality > 100):
        print(f'JPG质量系数超出范围！无法保存！')
        sys.exit()  # 结束程序

    if (png_quality < 0 or png_quality > 9):
        print(f'PNG质量系数超出范围！无法保存！')
        sys.exit()  # 结束程序

    if (is_resizeFrame):
        resize_frame = list(map(int, resize_frame.split(',')))  # 字符串转列表
        # 重塑视频帧尺寸
        w_resize = int(resize_frame[0] * resizeRatio_frame)  # 重塑宽度
        h_resize = int(resize_frame[1] * resizeRatio_frame)  # 重塑高度
        frame = cv2.resize(frame, (w_resize, h_resize),
                           interpolation=cv2.INTER_AREA)  # 重塑

    if (frame_saveStyle == 'jpg'):
        # 写入jpg文件
        cv2.imwrite(f'./{frame_savePath}/{frame_namePrefix}-{frame_num}.{frame_saveStyle}',
                    frame, [int(cv2.IMWRITE_JPEG_QUALITY), jpg_quality])
    elif (frame_saveStyle == 'png'):
        # 写入png文件
        cv2.imwrite(f'./{frame_savePath}/{frame_namePrefix}-{frame_num}.{frame_saveStyle}',
                    frame, [int(cv2.IMWRITE_PNG_COMPRESSION), png_quality])
    else:
        print(f'帧格式有问题！无法保存！')
        sys.exit()  # 结束程序


# 帧转换
def frames_transform(frame, countdown_msg, textFont, xy_c, font_size):

    frame_img = Image.fromarray(frame)  # array2PIL
    # 加入倒计时提示信息
    ImageDraw.Draw(frame_img).multiline_text(
        (xy_c[0]-font_size*13/2, xy_c[1]-font_size*3/2), countdown_msg,
        fill=(205, 250, 255), font=textFont, align="center")
    frame_array = np.array(frame_img)  # PIL2array

    return frame_array