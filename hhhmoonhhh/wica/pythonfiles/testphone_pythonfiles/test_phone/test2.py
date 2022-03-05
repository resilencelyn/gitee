# 引用uiautomator2包
import uiautomator2 as u2
import time
import matplotlib.pyplot as plt
import cv2
 

def draw_circle(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDBLCLK:
        cv2.circle(shrink, (x, y), 100, (255, 0, 0), -1)

 
def on_EVENT_LBUTTONDOWN(event, x, y, flags, param):
    if event == cv2.EVENT_LBUTTONDOWN:
        xy = "%d,%d" % (x, y)
        # a.append(x)
        # b.append(y)
        cv2.circle(shrink, (x, y), 1, (0, 0, 255), thickness=-1)
        cv2.putText(shrink, xy, (x, y), cv2.FONT_HERSHEY_PLAIN,
                    1.0, (0, 0, 0), thickness=1)
        
        cv2.imshow("image", shrink)
        print(x,y)


# 使用设备唯一标志码链接设备，其中9phqaetw是通过adb获取的设备标志码
d = u2.connect('MYQNW20402011777')  
# d = u2.connect()  # 当前只有一个设备时可以用这个
 
print(d.info)
# d(text="QQ").click()

# time.sleep(2)
# d.press("home")
print(d.window_size())
print()
d.click(351 / 0.4, 112 / 0.4)
time.sleep(0.2)
d.click(208 / 0.4, 209 / 0.4)
time.sleep(0.2)
d.click(388 / 0.4, 253 / 0.4)
time.sleep(0.2)
d.click(229 / 0.4, 719 / 0.4)
time.sleep(0.2)
d.click(229 / 0.4, 719 / 0.4)
time.sleep(0.2)
d.click(229 / 0.4, 719 / 0.4)
time.sleep(0.2)
d.click(229 / 0.4, 719 / 0.4)
time.sleep(0.2)
d.click(229 / 0.4, 719 / 0.4)

# time.sleep(0.2)
d.screenshot("test.jpg")
# time.sleep(5)
# d.press("back")
# d.screenshot("test.jpg")

img = cv2.imread('C:\\Users\\1\\Desktop\\test_phone\\test.jpg')
# print(img.shape)
height, width = img.shape[:2]  
print(height)
print(width)
# cv2.imshow('image',img)
size = (int(width*0.4), int(height*0.4))  
shrink = cv2.resize(img, size, interpolation=cv2.INTER_AREA)  

cv2.namedWindow("image")
# cv2.setMouseCallback('image', draw_circle)
cv2.imshow('image',shrink)
# cv2.setMouseCallback('image', draw_circle)

cv2.setMouseCallback("image", on_EVENT_LBUTTONDOWN)


k = cv2.waitKey(0)
if k==27:
    cv2.destroyAllWindows()  #wait for ESC key to exit




# img = plt.imread("C:\\Users\\1\\Desktop\\test_phone\\test.jpg")
# plt.imshow(img)
# plt.show()


# d.press("back")
# d.screen_on()
# d.screen_off()
# d.click(100,100)
# print(d.current_app)