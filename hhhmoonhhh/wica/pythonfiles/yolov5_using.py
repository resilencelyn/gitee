import os
import sys
import time


def using_yolov5(phone_ip: str):  # string.
    # C:\Users\1\Desktop\wica\tools\yolo\yolov5-master\yolov5-master>python detect.py --source http://admin:admin@192.168.0.104:8081
    """
    phone_ip -> the ip of the phone.
    using the yolov5(through the phone camera...)
    return none.
    """
    # C:\Users\1\Desktop\wica\tools\yolo\yolov5-master\yolov5-master>python detect.py --source http://admin:admin@192.168.0.104:8081
    # run the cmd commands.
    cmd_command_1 = f"cd&&cd tools&&cd yolo&&cd yolov5-master&&cd yolov5-master&&python detect.py --source http://admin:admin@{phone_ip}:8081"
    os.system(cmd_command_1)
    # run the yolov5 -> detect.py to use the yolov5 on the phone..
    # C:\Users\1\Desktop\wica\tools\yolo\yolov5-master\yolov5-master>python detect.py --source http://admin:admin@192.168.0.104:8081
    return None


