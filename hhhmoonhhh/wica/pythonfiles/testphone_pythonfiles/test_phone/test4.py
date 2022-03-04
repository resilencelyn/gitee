from re import T
import uiautomator2 as u2
import time
import cv2


d = u2.connect("192.168.0.104:5555")
# img = d.screenshot()
# cv2.namedWindow('image',cv2.WINDOW_NORMAL)
# cv2.imshow('image',img)
# cv2.waitKey(0)
# cv2.destroyAllWindows()

cv2.namedWindow("image")
while True:
    res = d.screenshot("test4.jpg")  # class
    print(res)
    img = cv2.imread('C:\\Users\\1\\Desktop\\test_phone\\test4.jpg')
    height, width = img.shape[:2]  
    size = (int(width*0.4), int(height*0.4))  
    shrink = cv2.resize(img, size, interpolation=cv2.INTER_AREA)  
    
    cv2.imshow('image',shrink)

    if cv2.waitKey(1) &0xFF ==ord('q'):  #按q键退出
    	break

cv2.destroyAllWindows()