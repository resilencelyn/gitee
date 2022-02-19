# 字体管理
# 创建人：曾逸夫
# 创建时间：2022-01-27


import os
import sys

ROOT_PATH = sys.path[0]  # 项目根目录

fonts_list = ['SimSun.ttc', 'TimesNewRoman.ttf']  # 字体列表
fonts_suffix = ['ttc', 'ttf', 'otf']  # 字体后缀


# 创建字体库
def add_fronts(fonts_file=f'{ROOT_PATH}/fonts.sh'):
    os.system(f'bash {fonts_file}')  # 执行下载字体库文件bash指令
    print(f'字体文件加载完成！')


# 判断字体文件
def is_fonts(fonts_dir):
    if (os.path.isdir(fonts_dir)):
        # 如果字体库存在
        fonts_flag = 0  # 判别标志
        f_list = os.listdir(fonts_dir)  # 本地字体库

        for i in fonts_list:
            if (i not in f_list):
                fonts_flag = 1  # 字体不存在

        if (fonts_flag == 1):
            # 字体不存在
            print(f'字体不存在，正在加载。。。')
            add_fronts()  # 创建字体库
        else:
            print(f'{fonts_list}字体已存在！')
    else:
        # 字体库不存在，创建字体库
        print(f'字体库不存在，正在创建。。。')
        add_fronts()  # 创建字体库
