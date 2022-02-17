# 快捷键判断
# 创建人：曾逸夫
# 创建时间：2022-02-13

import sys
from collections import Counter


# 快捷键冲突判断
def hotkey_judge(keyList):
    key_dict = dict(Counter(keyList))  # 快捷键统计
    # 判断快捷键是否冲突
    repeat_key = [key for key, value in key_dict.items() if value > 1]
    if (repeat_key != []):
        print(f'快捷键冲突! 程序结束！')
        sys.exit()  # 结束程序
