# 引用uiautomator2包
import uiautomator2 as u2
import time
import matplotlib.pyplot as plt
import cv2
 


# 使用设备唯一标志码链接设备，其中9phqaetw是通过adb获取的设备标志码
d = u2.connect('192.168.0.104:5555')  
# d.press("search")
d.clear_text()
d.send_keys("hello")
d.press("enter")
# d.keyevent("enter")

print(d.device_info)
print(d.info)
# print(dict({d.device_info, d.info}))
dict0 = {
    "device_info": d.device_info,
    "info": d.info
}
print(dict0)

print(d.settings)

print(d.app_list_running())

res = d.healthcheck()
print(res)
time.sleep(3)

d.press("recent")
time.sleep(3)
d.open_notification()
time.sleep(3)
d.open_quick_settings()
d.press("power")
# d.press("menu")
# d.press("camera")
# d.screen_on()
# d.unlock()
d.press("volume_up")
d.press("volume_up")
d.press("volume_down")
d.press("volume_down")
d.press("volume_up")


d.press("volume_up")
d.press("volume_up")


d.press("volume_up")
d.press("volume_up")
# d.press("volume_up")
# d.press("volume_up")
# d.press("volume_up")
d.press("volume_mute")
# d = u2.connect()  # 当前只有一个设备时可以用这个
d(text="拼多多").click()

d.press("back")
time.sleep(0.1)
d.press("back")
time.sleep(0.1)
d.press("back")

d.swipe(100, 600, 720, 600)
time.sleep(5)
d.swipe(720, 600, 100, 600)

print(d.info)
# d(text="QQ").click()
# d(scrollable=True).scroll.forward()
d(text="QQ").click()
d(scrollable=True).scroll.toEnd()
d.press("home")

d(text="微信").click()
# d(scrollable=True).scroll.toEnd()
d.swipe(500, 900, 500, 200)
d.press("back")

